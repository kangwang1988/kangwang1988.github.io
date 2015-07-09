# 代码片段
#### To check if a directory exists in a shell script you can use the following:
	
	if [ -d "$DIRECTORY" ]; then
	# Control will enter here if $DIRECTORY exists.
	fi
	Or to check if a directory doesn't exist:
	if [ ! -d "$DIRECTORY" ]; then
	# Control will enter here if $DIRECTORY doesn't exist.
	fi
	
#### In general to concatenate two variables you can just write them one after another:

	a='hello'
	b='world'
	c=$a$b
	echo $c
	helloworld
	
#### SVN checkout the contents of a folder, not the folder itself

	svn checkout file:///home/landonwinters/svn/waterproject/trunk
	
#### SVN not updating recursively

	svn up --depth infinity
	svn up --set-depth infinity
	
#### start mySQL server from command line on Mac OS Lion

	shell> sudo /usr/local/mysql/bin/mysqld_safe
	(Enter your password, if necessary)
	(Press Control-Z)
	shell> bg
	(Press Control-D or enter "exit" to exit the shell)
	
#### The following (save as rr.sh) reads a file from the command line:

	#!/bin/bash
	while read line
	do
	name=$line
	echo "Text read from file - $name"
	done < $1

#### search a string in Shell script variable

	flag=`echo $a|awk '{print match($0,"pass")}'`;
	if [ $flag -gt 0 ];then
	echo "Success";
	else
	echo "fail";
	fi

#### generate ssl cert & key for apache

	openssl genrsa 1024 >server.key
	openssl req -new -key server.key > server.csr
	openssl req -x509 -days 3650 -key server.key -in 	server.csr >server.crt

#### Mysql CREATE TABLE,Primary key,insert record
	
	CREATE TABLE Persons
	P_Id int NOT NULL,
	LastName varchar(255) NOT NULL,
	FirstName varchar(255),
	Address varchar(255),
	City varchar(255),
	PRIMARY KEY (P_Id))
	INSERT INTO table_name (column1,column2,column3,...) VALUES (value1,value2,value3,...);

#### Set / change / reset the MySQL root password on Ubuntu Linux. Enter the following lines in your terminal.

	1.Stop the MySQL Server.
	sudo /etc/init.d/mysql stop
	2.Start the mysqld configuration.
	sudo mysqld --skip-grant-tables &
	3.Login to MySQL as root.
	mysql -u root mysql
	4.Replace YOURNEWPASSWORD with your new password! UPDATE user SET Password=PASSWORD('YOURNEWPASSWORD') WHERE User='root'; FLUSH PRIVILEGES; exit;

#### PHP Connect to the MySQL Server

	<?php
	// Create connection$con=mysqli_connect("example.com","peter","abc123","my_db");
	// Check connection
	if (mysqli_connect_errno()) {
	echo "Failed to connect to MySQL: " . 	mysqli_connect_error();}
	$result = mysqli_query($con,"SELECT * FROM Persons");
	while($row = mysqli_fetch_array($result)) {
	echo $row['FirstName'] . " " . $row['LastName'];
	}
	mysqli_close($con);
	?>

#### SQL - IF EXISTS UPDATE ELSE INSERT INTO
	
	1.Create a UNIQUE constraint on your subs_email column, if one does not already exist:
	ALTER TABLE subs ADD UNIQUE (subs_email)
	2.Use INSERT ... ON DUPLICATE KEY UPDATE:
	INSERT INTO subs (subs_name, subs_email, subs_birthday) VALUES (?, ?, ?)
	ON DUPLICATE KEY UPDATE
	subs_name = VALUES(subs_name),
	subs_birthday = VALUES(subs_birthday)

#### Reading HTML content from a UIWebView
	NSString *html = [yourWebView	stringByEvaluatingJavaScriptFromString:
	@"document.body.innerHTML"];

#### Cropping a UIImage
	CGImageRef imageRef = CGImageCreateWithImageInRect([largeImage CGImage], cropRect);
	// or use the UIImage wherever you like
	[UIImageView setImage:[UIImage imageWithCGImage:imageRef]];
	CGImageRelease(imageRef);

#### How to change Status Bar text color in iOS 7.
	Set the UIViewControllerBasedStatusBarAppearance to YES in the .plist file.
	In the viewDidLoad do a [self 	setNeedsStatusBarAppearanceUpdate];
	Add the following method:
	-(UIStatusBarStyle)preferredStatusBarStyle{
	return UIStatusBarStyleLightContent;
	}
	
#### makeObjectsPerformSelector:
	NSArray *newArray = [[NSArray alloc] initWithArray:oldArray copyItems:YES];
	[newArray makeObjectsPerformSelector:@selector(doSomethingToObject)];
#### iOS - Identify what UIViewController was previously shown in a UINavigationController
	- (void)navigationController:(UINavigationController *)navigationController willShowViewController:(UIViewController *)viewController animated:(BOOL)animated

#### Efficiently finding the average color of a UIImage
	#import "UIImage+AverageColor.h"
	@implementation UIImage (AverageColor)
	- (UIColor *)averageColor {
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	unsigned char rgba[4];
	CGContextRef context = CGBitmapContextCreate(rgba, 1, 1, 8, 4, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
	CGContextDrawImage(context, CGRectMake(0, 0, 1, 1), self.CGImage);
	CGColorSpaceRelease(colorSpace);
	CGContextRelease(context);
	if(rgba[3] &gt; 0) {
	CGFloat alpha = ((CGFloat)rgba[3])/255.0;
	CGFloat multiplier = alpha/255.0;
	return [UIColor colorWithRed:	((CGFloat)rgba[0])*multiplier
	green:((CGFloat)rgba[1])*multiplier
	blue:((CGFloat)rgba[2])*multiplier
	alpha:alpha];
	}
	else{
	return [UIColor colorWithRed:((CGFloat)rgba[0])/255.0
	green:((CGFloat)rgba[1])/255.0
	blue:((CGFloat)rgba[2])/255.0
	alpha:((CGFloat)rgba[3])/255.0];
	}
	}
	@end
#### Curl 
	GET with JSON:
	curl -i -H "Accept: application/json" -H "Content-Type: application/json" http://hostname/resource
	with XML:
	curl -H "Accept: application/xml" -H "Content-Type: application/xml" -X GET http://hostname/resource
	POST:
	For posting data:
	curl --data "param1=value1&param2=value2" http://hostname/resource
	For file upload:
	curl --form "fileupload=@filename.txt" http://hostname/resource
	RESTful HTTP Post:
	curl -X POST -d @filename http://hostname/resource
	For logging into a site (auth):
	curl -d "username=admin&password=admin&submit=Login" --dump-header headers http://localhost/Login
	curl -L -b headers http://localhost/
#### .a Opertion
	lipo -info xx.a
	creates or operate on fat (multi-architecture) files.
#### 雪球Data
	curl --header "Cookie: xq_a_token=tokenvalue;" http://api.xueqiu.com/v4/stock/quote.json?code=SHcode