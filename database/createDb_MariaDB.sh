#!/usr/bin/env python

import pexpect

PASSWORD='passwd'
# mysqladmin -u root password passwd

def waitForPrompt(mysql, timeout):
	mysql.expect('.+>', timeout)
        print(mysql.before)

def main():
	# Safety
	selection = raw_input('Confirm re-creating database (y/n): ')
	if selection != 'y':
		return

	print("re-creating database...")
	try:
		# Login
		mysql = pexpect.spawn('mysql -u root media_archive')
		#mysql.expect('Enter password:')
		#mysql.sendline(PASSWORD)
		waitForPrompt(mysql, 5)

		# Remove existing tables
		mysql.sendline('DROP TABLE avp;')
		waitForPrompt(mysql, 5)

                mysql.sendline('DROP TABLE tag;')
                waitForPrompt(mysql, 5)

                mysql.sendline('DROP TABLE object;')
                waitForPrompt(mysql, 5)

                mysql.sendline('DROP TABLE suffix;')
                waitForPrompt(mysql, 5)

		# Create tables

                sql = "CREATE TABLE avp ("
                sql += "id INT UNSIGNED NOT NULL AUTO_INCREMENT,"
                sql += "attribute VARCHAR(50) NOT NULL,"
                sql += "value VARCHAR(132) NOT NULL,"
                sql += "CONSTRAINT avp_pk PRIMARY KEY (id),"
                sql += "CONSTRAINT avp_uk UNIQUE KEY (attribute, value)"
                sql += ")ROW_FORMAT=DYNAMIC;"
                print(sql)
		mysql.sendline(sql)
		waitForPrompt(mysql, 5)

                sql = 'CREATE TABLE suffix ('
                sql += 'id INT UNSIGNED NOT NULL AUTO_INCREMENT,'
                sql += 'suffix VARCHAR(132) NOT NULL,'
                sql += 'CONSTRAINT suffix_pk PRIMARY KEY (id),'
                sql += 'CONSTRAINT suffix_uk UNIQUE KEY (suffix)'
                sql += ');'
                print(sql)
                mysql.sendline(sql)
                waitForPrompt(mysql, 5)

                sql = 'CREATE TABLE object ('
                sql += 'id INT UNSIGNED NOT NULL AUTO_INCREMENT,'
                sql += 'sha1 VARCHAR(64) NOT NULL,'
                sql += 'access_counter INT UNSIGNED NOT NULL,'
                sql += 'suffix_id INT UNSIGNED NOT NULL,'
                sql += 'CONSTRAINT object_pk PRIMARY KEY (id),'
                sql += 'CONSTRAINT object_fk FOREIGN KEY (suffix_id) REFERENCES suffix(id) ON UPDATE CASCADE,'
                sql += 'CONSTRAINT object_uk UNIQUE KEY (sha1)'
                sql += ');'
                print(sql)
                mysql.sendline(sql)
                waitForPrompt(mysql, 5)

                sql = 'CREATE TABLE tag ('
                sql += 'id INT UNSIGNED NOT NULL AUTO_INCREMENT,'
                sql += 'obj_id INT UNSIGNED NOT NULL,'
                sql += 'avp_id INT UNSIGNED NOT NULL,'
                sql += 'CONSTRAINT tag_pk PRIMARY KEY (id),'
                sql += 'CONSTRAINT tag_fk FOREIGN KEY (obj_id) REFERENCES object(id) ON UPDATE CASCADE,'
                sql += 'CONSTRAINT tag_uk UNIQUE KEY (obj_id, avp_id)'
                sql += ');'
                print(sql)
                mysql.sendline(sql)
                waitForPrompt(mysql, 5)

		mysql.close()
		print("done!")
	except pexpect.TIMEOUT:
		print('Error: Timeout when communicating to database')

main()
