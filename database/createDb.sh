#!/usr/bin/env python

import pexpect

PASSWORD='5b4ZgH12'

def waitForPrompt(mysql, timeout):
	mysql.expect('.+mysql>', timeout)

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

		# Create tables
		sql = 'CREATE TABLE avp ('
		sql += 'avp_id INT UNSIGNED NOT NULL AUTO_INCREMENT,'
		sql += 'attribute VARCHAR(255) NOT NULL,'
		sql += 'value VARCHAR(255) NOT NULL,'
		sql += 'PRIMARY KEY (avp_id),'
                sql += 'UNIQUE KEY unique_key_constraint (attribute, value)'
		sql += ');'
                print(sql)
		mysql.sendline(sql)
		waitForPrompt(mysql, 5)

                sql = 'CREATE TABLE object ('
                sql += 'obj_id INT UNSIGNED NOT NULL AUTO_INCREMENT,'
                sql += 'sha1 VARCHAR(64) NOT NULL,'
                sql += 'PRIMARY KEY (obj_id),'
                sql += 'UNIQUE KEY unique_key_constraint (sha1)'
                sql += ');'
                print(sql)
                mysql.sendline(sql)
                waitForPrompt(mysql, 5)

                sql = 'CREATE TABLE tag ('
                sql += 'tag_id INT UNSIGNED NOT NULL AUTO_INCREMENT,'
                sql += 'obj_id INT UNSIGNED NOT NULL,'
                sql += 'avp_id INT UNSIGNED NOT NULL,'
                sql += 'PRIMARY KEY (tag_id),'
                sql += 'FOREIGN KEY (obj_id) REFERENCES object(obj_id) ON UPDATE CASCADE,'
                sql += 'UNIQUE KEY unique_key_constraints (obj_id, avp_id)'
                sql += ');'
                print(sql)
                mysql.sendline(sql)
                waitForPrompt(mysql, 5)

		mysql.close()
		print("done!")
	except pexpect.TIMEOUT:
		print('Error: Timeout when communicating to database')

main()
