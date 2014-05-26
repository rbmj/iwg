www Setup Explanation
=====================

Overview
--------
Our web server setup was pretty straightforward. We used a standard LAMP stack to set up the server, openssl to support https, and phpbb to set up a web forum. We didn't have any major tricks or strategies for security. Instead, we followed the "keep it simple, stupid" mindset and made config changes as they came up. The forum and MySQL database are too heavy to merit putting on github, but we did retain all the apache configs.

Gotta give a shout out to whomever creates the Linode Library, because their tutorials are top notch.

</p>
LAMP Stack
----------
We followed the guide at https://library.linode.com/lamp-guides/debian-7-wheezy to setup the basic services required to host a web page and forum.

We also had to enable a static ipv6 address, as per the CDX specifications. The guide at http://www.cyberciti.biz/faq/ubuntu-ipv6-networking-configuration/ explains how to do it. Apache, by default, listens on all ip addresses, so it didn't need to be modified to allow ipv6.

</p>
apache
------
Details of our configuration can be found in the config files. Besides setting the main page to be at /var/www, not much needed to be done.

</p>
ssl
---
We used openssl with self-signed certificates. We used a variety of now unknown tutorials to do this, but the guide at http://www.onlamp.com/2008/03/04/step-by-step-configuring-ssl-under-apache.html is basically the same as what we did. Because heartbleed did not put any of our availability points at risk, we didn't bother to patch openssl when heartbleed broke during the exercise.

**NOTE:** There's a non-negligible chance that heartbleed will end up changing how openssl should be setup within the next year. I would recommend picking a new, recent guide from scratch next year in order to properly set up ssl.

</p>
phpbb
-----
Phpbb is a free, simple, robust application. To set it up, we installed MySQL according to https://library.linode.com/databases/mysql/debian-7-wheezy and then installed phpbb according to https://library.linode.com/web-applications/bulletin-boards/phpbb/debian-6-squeeze. Even after the initial installation, we played around with all the phpbb settings in order to secure the board better.

----------------------
**-Zane**
