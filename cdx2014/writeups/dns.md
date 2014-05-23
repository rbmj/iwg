DNS Setup Explanation
=====================

Strategy
--------
</p>

In designing our CDX 2014 nameserver, we based our decisions on the following principles:

1. Provide the minimal satisfactory service.
2. Make the system easy to maintain.
3. Secure the system as much as reasonably possible.

Our overall setup was to use our Active Directory Domain Controller as a private master and to use a BIND nameserver as a publicly resolving slave to the AD DC. We made this choice for several reasons. First, it was easier to set up and maintain. Second, having two masters for the same domain would require a handful of hackish configurations that could have potentially opened up security holes. Finally, there were no significant security drawbacks. It would have been easier for red cell to find IP's of our private machines. But, because they had non-standard names, red cell would have only found them through lucky guesses on nslookup, which would be no faster than a standard nmap.

Details
-------
</p>

The exact configuration is included in this repo. Here, I will provide some explanation for our choices in the config files.

In options, we
- set version to "not currently available" to slow recon
- allowed only the AD DC to notify (send updates) as per decision principle 1.
- Disabled zone transfers to prevent red cell from easily obtaining our network configuration.
- Allowed recursion through either the main AD DC or the backup. We also forwarded to hq's nameserver. These options were required in order to successfully resolve external queries, which we had to allow.

In local, we just made one zone for our domain. We played around with logging reverse zone lookups, but found them to be unnecessary, thus breaking decision principle 1.

Recommendations
---------------
</p>

Overall, this setup worked very successfully. DNS did end up being exploited through DNS back channels, but that would have required a snort fix vice a BIND fix. 

AD's DNS manager is easy to use, but, because using AD introduces so many complications in general, it would be preferable if we could get rid of AD entirely next year. In that case, we could use a simpler BIND master-slave setup.

Custom logging could save a lot of time. We just didn't get around to it this year, which meant that we spent a lot of time poring through the awfully-formatted syslog.

Recommended Resources
---------------------
</p>
The [zytrax](http://www.zytrax.com/books/dns/) site was invaluable. 

Wish I had learned about [named-checkconf](http://linux.die.net/man/8/named-checkconf) and [named-checkzone](http://ftp.isc.org/www/bind/arm95/man.named-checkzone.html) months earlier.

dig @NAMESERVER DOMAINNAME returns the DNS records for a query.

nslookup - NAMESERVER puts you in a prompt where you can interactively nslookup on the specified NAMESERVER.


----------------------
**-Zane**
