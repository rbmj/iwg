Setting Up Active Directory on Linux
------------------------------------

I'm going to assume Debian, but it should work with some modifications
on any distribution

You will need:
 - Debian Packages:
    - krb5-user
    - libpam-krb5
    - winbind
    - libnss-winbind
    - ntpd
 - RPM Packages:
    - ntp
    - krb5-workstation
    - pam_krb5
    - samba3x-winbind

The machine needs to have a valid FQDN in /etc/hosts

First, create a machine account on the DC.  This can be done by
going to start->Admin tools->AD Users and Computers, then clicking
on the desired ``folder'' for the computer on the left (I chose
usna.bluenet/Servers), and then right click new->computer.  The
computer name needs to match the computer's hostname exactly.

AD is very picky about the time, so you need to make sure you are
correctly synced with the NTP server.  Here's a /etc/ntp.conf:

    driftfile /var/lib/ntp/ntp.drift
    statistics loopstats peerstats clockstats
    filegen loopstats file loopstats type day enable
    filegen peerstats file peerstats type day enable
    filegen clockstats file clockstats type day enable

    # Put your ntp server here
    # It might not be the domain controller - they don't have to
    # be the same computer (might be e.g. ntp.usna.bluenet)
    server dc0.usna.bluenet

    restrict -4 default kod notrap nomodify nopeer noquery
    restrict -6 default kod notrap nomodify nopeer noquery
    restrict 127.0.0.1
    restrict ::1

With openntpd, all you need is the server line.

Then, make sure ntp is not running, force-update the time,
and start the ntp daemon:

    # service ntp stop
    # ntpd -gq
    # service ntp start

On systems with openntpd, you want `openntpd -qs`.  You can also use
`ntpdate SERVER`.  The service daemon may be named any of `ntp`,
`ntpd`, `openntp`, or `openntpd`.

In order to do anything with AD, you need a kerberos ticket.
Here's a sample /etc/krb5.conf:

    [logging]
            default = FILE:/var/log/krb5libs.log
            kdc = FILE:/var/log/krb5kdc.log
            admin_server = FILE:/var/log/kadmind.log

    [libdefaults]
            default_realm = USNA.BLUENET

    [realms]
            USNA.BLUENET = {
                    kdc = dc0.usna.bluenet
                    admin_server = dc0.usna.bluenet
            }

    [domain_realm]
            .usna.bluenet = USNA.BLUENET
            usna.bluenet = USNA.BLUENET

Note that everything to do with AD is case sensitive - UPPER CASE
is important.

You should then be able to get a ticket by issuing:

    # kinit blue@USNA.BLUENET

where `blue' is a user that is a domain admin.  To check this worked
correctly, use klist:

    # klist
    Ticket cache: FILE:/tmp/krb5cc_0
    Default principal: blue@USNA.BLUENET

    Valid starting    Expires           Service principal
    02/04/2014 20:13  03/04/2014 06:14  krbtgt/USNA.BLUENET@USNA.BLUENET
            renew until 03/04/2014 20:13

Next, you need to configure winbind.  winbind shares a configuration
file with samba, but you do NOT need to install samba - that's just
an additional attack vector.  Here's a sample /etc/samba/smb.conf:

    [global]
            security = ads
            realm = USNA.BLUENET
            workgroup = USNA
            # winbind needs this but we wont' use this
            idmap config * : backend = tdb
            idmap config * : range = 90000-99999
            # don't collide with system accounts
            idmap config USNA : backend = ad
            idmap config USNA : range = 100000-200000
            winbind enum users = yes
            winbind enum groups = yes
            client use spnego = yes
            client ntlmv2 auth = yes
            client ldap sasl wrapping = sign
            encrypt passwords = yes
            winbind use default domain = yes
            # needed to ensure users have a login shell
            template shell = /bin/bash
            # Don't allow anonymous connections
            restrict anonymous = 2

You can check the configuration using the `testparm` command.

Restart winbind:

    # service winbind restart

Now you should be able to join the domain:

    # net ads join -U blue@USNA.BLUENET -W USNA.BLUENET

You might get a DNS error here - that is normal.  Some versions
do not want the `@USNA.BLUENET` at the -U argument, so if you get
errors about `malformed representation of principal`, try:

    # net ads join -U blue -W USNA.BLUENET

Now you need to restart winbind again

    # service winbind restart

Test that the join indeed succeeded:

    # wbinfo -u
    [list of domain users - should NOT BE EMPTY]

Now, you need to set up logins for domain users.  The steps are
different on debian and rpm based distributions.  On debian:

START DEBIAN SPECIFIC INSTRUCTIONS

First allow authentication using kerberos:

    # pam-auth-update
    [ Select Kerberos && Unix, plus others if they are necessary ]
    [               winbind is NOT necessary here                ]

Domain users will not have home directories, so make sure that
they get created when they first login by adding the following
line with the other session commands in /etc/pam.d/login:

    session required pam_mkhomedir.so

NOTE:  There should be a debian-specific configure script that enables
this.  However, it seems like the patch is still being discussed and has
not made it to the repos yet as they are arguing over the best way to
implement it.  Editing manually is itself rather pessimal as it screws
up autogeneration of PAM configs (which is a GOOD thing as PAM is quite
complicated...).

Finally, you need to make sure that the users and groups
are loaded by winbind by changing /etc/nsswitch.conf:

    passwd:		compat winbind
    group:		compat winbind
    shadow:		compat

    hosts:		files mdns4_minimal [NOTFOUND=return] dns mdns4
    networks:	files

    protocols:	db files
    services:	db files
    ethers:		db files
    rpc:		db files

    netgroup:	nis

END DEBIAN SPECIFIC INSTRUCTIONS

On RPM distributions:

START RPM SPECIFIC INSTRUCTIONS

The config script on RHEL and CentOS for PAM is quite nice, and so this
is just one shell command.  Debian's working on catching up in this area.

    # authconfig --enablekrb5 --enablemkhomedir --enablewinbind --update

I don't have any instructions for other RPM based distributions
(SUSE, etc.), so that might need to be added later.

END RPM SPECIFIC INSTRUCTIONS

RPM and Debian converge here:

And finally, if you want domain administrators to be able to
sudo on the box, you need to add the following to sudoers:

    # visudo
    [ Add the following line with the other grants: ]
    %domain\ admins ALL=(root) ALL

Note that this lets domain admins do whatever they want as
root - this might be much more than you want!

You can test by logging in as the user that you used to join
the domain, verifying that you log in correctly, your home
directory is set up properly, and that you have sudo access.
If all of this works, then you're good!  If not, your best
sources of information on what went wrong are going to be
/var/log/auth.log and the logs in /var/log/samba.

Godspeed.  AD sucks.
