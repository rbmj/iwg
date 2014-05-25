www Setup Explanation
=====================

Overview
--------
Our web server setup was pretty straightforward. We used a standard LAMP stack to set up the server, openssl to support https, and phpbb3 to set up a web forum. We didn't have any major tricks or strategies for security. Instead, we followed the "keep it simple, stupid" mindset and made config changes as they came up. The forum and MySQL database are too heavy to merit putting on github, but we did retain all the apache configs.

Gotta give a shout out to whomever creates the Linode Library, because their tutorials are top notch.

</p>
LAMP Stack
----------
We followed the guide at https://library.linode.com/lamp-guides/debian-7-wheezy to setup the basic services required to host a web page and forum.

We also had to enable a static ipv6 address, as per the CDX specifications. The guide at http://www.cyberciti.biz/faq/ubuntu-ipv6-networking-configuration/ explains how to do it. Apache, by default, listens on all ip addresses, so it didn't need to be modified to allow ipv6.

</p>
apache
------


</p>
ssl
---

</p>
phpbb3
------

----------------------
**-Zane**
