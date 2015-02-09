USNA CDX Email Architecture 2014
================================

Introduction
------------

This document is an introduction to mail architecture in general and a
guide to setting up a cdx netspec-compliant mail server.  This guide
should have all of the information that you need, but that information
only works if you read the document.  It *is* rather long, yes, but there
are important bits of information throughout, though users deeply
familiar with how email works can skip the overview section and go directly
to the in-depth section for configuration details.

E-Mail has a rather strange architecture, and relies on several programs
working together to work correctly.  This document attempts to explain how
these various programs fit together, and how to configure them to obtain a
netspec-compliant mail server.

The backbone of the USNA mail architecture is the combination of two
complimentary servers:  Dovecot and Postfix.

Dovecot does several things for the email system and is the foundation of
the entire system.  It keeps track of email account information (similar
to the role of /etc/passwd), provides a mechanism for the other mail
programs to authenticate mail accounts, manages user's mailboxes, provides
a mechanism for other mail programs to deliver mail to a user's mailbox,
and runs the servers from which clients retrieve their mail.

Postfix is a complimentary program.  It provides the interface for clients
to send mail, makes mail routing decisions, provides filters to determine
what mail is forwarded/delivered, offers hooks for other programs to analyze
mail as it flows through the system and reject it (e.g. antivirus), and
delivers incoming mail to dovecot for management.

Overview of Email Infrastructure
--------------------------------

A typical email is sent as follows.  First, a user uses a Mail User Agent
(MUA, e.g. Thunderbird) to compose and send the e-mail.  The MUA then
contacts the user's e-mail provider over the SMTP protocol.  In our case,
it would be contacting Postfix.  This transaction is done over the standard
SMTP port 25.  Their provider's e-mail server, after authenticating the user
(which in our case is done by checking with Dovecot), determines that the
mail should be sent.  It then performs a DNS query for the recipient(s)
domain(s)'s MX, or Mail eXchanger, record(s).  It contacts this server using
SMTP and sends the message, but this time over port 587.  That server, after
determining that the mail should be delivered, contacts its mail repository
(Dovecot in our case), which delivers the mail.  The recipient then uses
their MUA to contact their e-mail provider over the POP3 or IMAP protocols
and receives mail.

The flexibility and also glaring weakness is that e-mail does not necessarily
need to conform to this model.  It can perform an arbitrary number of hops on
the way to its destination, even zero in the case of a recipient on the same
domain as the sender.  The result of this is that any given server has no way
of determining whether the mail is legitimate.  It could have been forged or
tampered with by any server at any link in the routing chain.  The normal
solution is that parties sign their mail to ensure that it is legitimate, but
not all users send their mail, and rejecting all unsigned mail causes more
problems than it solves, at least at this time.  Authentication of original
senders of mail via user/password combined with SSL allows the original server
to know that the mail is legitimate.  Major MX servers can then operate
something akin to a web of trust (we will forward your mail as long as you
promise to only send out legit mail, and we can trust you).

Basic security practices to avoid embarssing leaks of spam or forged mail
should be applied to mail filters.  A sane configuration for CDX, and what
was used for CDX 2014, was to allow authenticated users from the local
domain to send whatever mail they want to wherever they want.  It will also
deliver mail to local recipients from anywhere, with no promise of
authentication, so long as the client does not say they are on the local
domain without authenticating.  This means will allow forged mail to be
delivered to local recipients, but any mail leaving the server can
be trusted as legitimate.  To illustrate this more clearly:

	Incoming mail:
	   - Is the sender a member of the local domain (usna.bluenet)?
	      - (YES) Are they authenticated?
		     - (YES) ACCEPT.
			 - (NO) REJECT.
		  - (NO) Is the recipient a member of the local domain (usna.bluenet)?
		     - (YES) ACCEPT.
			 - (NO) REJECT.

Some email servers are configured such that a person can send email via
that server to any recipient claiming to be any sender.  These servers
are called Open Relays and are a serious problem for e-mail security.

In the real world, many servers operate a complex system of heuristics,
whitelists, and blacklists of different hosts along the relay chain in
the hopes of minimizing the amount of spam allowed to pass through the
system.  Some spam, however, invariably gets through.  Unless the entire
world decides to adopt PKI on a person-to-person level and have a web
of trust that can include most of the internet users, we cannot have total
confidence in any arbitrary piece of mail.  However, the current system
closely approximates this ideal.  Note also that this only means that
the messages are from a confirmed sender and recipient - it does not
gurantee that the messages have useful non-spam content.

CDX 2014 E-Mail Infrastructure In-Depth
---------------------------------------

First, we'll look at the setup for dovecot. At the beginning of the
config file, we need to allow plaintext authentication in order to conform
with the netspec.  The user that owns all of the mail files is given the
name 'mail', so we account for that as well. Mail will be stored in the
mail user's home directory, which is /var/mail.  The format for storing
mail is maildir, which is a standard filesystem mailbox format.  We need
two protocols to be enabled.    We need to provide two protocols.  IMAP
is used by the clients to check their mail (we could also use POP3 for this,
but netspec requires IMAP and POP3 is just redundant).  LMTP is the local
mail transport protocol, and is used by postfix to deliver mail to dovecot.

For authentication, we allow plain (necessary for netspec, but bad in
general as it is plaintext), login (another plaintext method added for
extra compatibility), and CRAM-MD5, an encrypted authentication method.
User accounts should be set up to authenticate using CRAM-MD5 if not using
TLS to encrypt the entire connection.

The passwords are stored in /etc/dovecot/domain/passwd. NOTE: some clients
include the domain while authenticating, and others do not.  With this
configuration, it is important to symlink /etc/dovecot/passwd to
/etc/dovecot/usna.bluenet/passwd, or to change the configuration.  Failure
to do this will result in some clients not being able to connect.

The password file is a fairly simple format similar to /etc/passwd with
user:pass pairs separated by newlines.  Passwords can be stored in plain
text; however, this is quite insecure.  Passwords can be hashed using the
`doveadm pw` tool.  Hashing a password in one format, however, will prevent
encrypted logins (but not plain-text over an encrypted connection) using
a different hash scheme.  The default hash is CRAM-MD5, which is the most
commonly supported.  MD5 is not a great hash, though, so this only provides
a small amount of additional security.

The userdb file is just more /etc/passwd type settings.  This section
needs expansion.  I do not recall doing much with this file last year
though.

The next two service lines set up private communcations channels between
postfix and dovecot.  These are unix filesystem sockets and they allow
postfix to use dovecot to authenticate and to deliver mail.  Having postfix
do authentication and mail delivery through dovecot greatly simplifies
matters.

The last two lines are the ssl certificate to use for encrypted connections.

Next, we look at postfix.  This configuration is slightly more involved.
The primary postfix configuration file is /etc/postfix/main.cf.
The first important configuration options are just to configure TLS. After
that, we have the necessary configuration options to allow postfix to
communicate with dovecot for authentication purposes.  We then disallow
anonymous connections, which is very important.  This still allows for
unauthenticated connections, but prevents anonymous users from authenticating
and gaining the same privileges as authenticated users.

After this, we then specify the rules for the built-in postfix filters.
These are mainly used to prevent spam and email-forgery.
smtpd_recipient_restrictions is the main configuration option here.  These
filters are applied in order.  Thus, the main filter that applies to all
mail after the recipient is known looks like:

	IF user is logged in, but sends mail from an address other than their own:
	   REJECT
	ELSE IF mail recipient is a local user, but that user does not exist:
	   REJECT
	ELSE IF user is authenticated:
	   ACCEPT
	ELSE IF mail recipient is a local user:
	   ACCEPT
	ELSE:
	   REJECT

The most important thing here is to prevent the server from acting as an
open relay.  This happens when any user, without authenticating, can send
mail to any recipient claiming to be any sender.  What we do here is more
conservative:  we allow any mail coming from outside our network to be
delivered to local addresses, but reject all mail from unauthenticated
local users or to locations outside our network.

NOTE 1:  smtpd_recipient_restrictions can specify restrictions on the sender;
the name of the option refers to the point in processing at which the
filtering occurs.

NOTE 2:  We probably should use reject_unlisted_sender as well here.

The smtpd_sender_restrictions that we use are less strict, and basically
just state that we will not accept mail from known bad sender addresses.

smtpd_relay_restrictions is a feature that is not available in the version
of postfix used for CDX2014.  In versions without this feature, all mail
is passed through smtpd_recipient_restrictions.  In versions with this
feature, relayed mail (i.e. mail that postfix does not perform final delivery
of) is passed through smtpd_relay_restrictions, while non-relayed mail
(which is forwarded to dovecot for delivery) uses the
smtpd_recipient_restrictions ruleset instead.

We then specify to run mail through the ClamAV daemon listening on local
port 10025.

The local domain configuration is relatively standard; look it up in the
postfix manual if something does not make sense.

The domain configuration specifies some important configuration options.
It specifies the location of the mailbox mappings, the alias mappings, 
the login mappings, and the transport mechanism for virtual mail delivery
(deliver to dovecot over LMTP).

All of the postfix map files are tab-delimited plain text files.  These
create hash tables that should be regenerated each time the file is edited.
These hash tables can be regenerated using the `postmap [file]` command.
They can be replaced by a number of different database types; see the
postfix manual for details.

The virtual mailbox mappings (/etc/postfix/vmailbox) specifies to postfix
which mailbox to deliver each user's mail to.  This should be set up to
match the dovecot configuration for virtual user mailbox location.  This
file is quite simple.  The format is 'user [tab] mailbox'.

The virtual alias mappings (/etc/postfix/virtual) specifies to postfix any
alias email addresses.  For example, mail that lists mail@usna.bluenet
as the recipient should be delivered to mason@usna.bluenet.  This is
for mail receipt purposes and can be recursive.  The format is
'alias address [tab] delivery address'.

The virtual login mappings (/etc/postfix/users) lists to postfix which
users should be allowed to send mail from which addresses.  It takes
the format 'sender address [tab] authorized users' where authorized
users are delimited by whitespace.

A lot of pain with this configuration is due to the fact that rubberneck
logs in slightly differently than most email clients.  This is why dovecot
needs the symlink of /etc/dovecot/passwd to /etc/dovecot/usna.bluenet/passwd
and why the rubberneck@usna.bluenet line in /etc/postfix/users needs both
rubberneck and rubberneck@usna.bluenet as authorized senders.

Fuck rubberneck.

The last postfix configuration file is /etc/postfix/master.cf.  This file
specifies which daemons to run with postfix.  The only difference here from
the default (mostly) is that we operate a submission client on port 587 to
be RFC compliant (basically run the SMTP server on an additional port) and
we add a line that runs an additional SMTP server on localhost for ClamAV.
This is so that once ClamAV has scanned mail it can inject mail back into
postfix.  Since messages have already been filtered from when they entered
the system we clear almost all of our filters for mail coming from ClamAV
except for some basic sanity checks.  Note that failing to clear filtering
coming back from ClamAV will cause quite a bit of mail to be incorrectly
rejected.

Workflow For Adding Users
-------------------------

In order to add a user to the system, one first uses `doveadm pw` to hash
their password, and then adds the relevant entry to the dovecot passwd
file /etc/dovecot/usna.bluenet/passwd.  After this, one adds a mapping
of user@usna.bluenet to usna.bluenet/user in /etc/postfix/vmailbox. Any
aliases alias@usna.bluenet to user@usna.bluenet should be added to
/etc/postfix/virtual.  Then, the sender permissions for the accounts
should be added (user@usna.bluenet user, alias@usna.bluenet user) to
/etc/postfix/users.  The postfix maps need to be regenerated then by
issuing `postmap /etc/postfix/{vmailbox,virtual,users}`.

Future Development
------------------

More advanced filtering can be done by incorporating the amavisd milter
(mail filter).  This offers many more options for things such as executable
breaking and custom filtering of files.

It is also possible to replace all of the flat files with databases using
either postgresql or mysql.  This was deemed unnecessary for CDX2014 but
could be useful if more scriptability was desired.

If it is desired, email credentials can be looked up via LDAP to synchronize
email credentials with AD credentials.  We decided not to go this route. PAM
system authentication may also achieve this same effect if the email server
is AD-joined and AD policies permit logons of all users on to the email
server (though this may be unwise).
 
