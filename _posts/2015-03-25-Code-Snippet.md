---
layout: post
title: Code Snippet
---

@HangZhou
<br>
================ Snippet Begin ================<br>
To check if a directory exists in a shell script you can use the following:<br>
if [ -d "$DIRECTORY" ]; then<br>
# Control will enter here if $DIRECTORY exists.<br>
fi<br>
Or to check if a directory doesn't exist:<br>
<br>
if [ ! -d "$DIRECTORY" ]; then<br>
# Control will enter here if $DIRECTORY doesn't exist.<br>
fi<br>
================ Snippet End ================<br>
================ Snippet Begin ================<br>
In general to concatenate two variables you can just write them one after another:<br>
<br>
a='hello'<br>
b='world'<br>
c=$a$b<br>
echo $c<br>
helloworld<br>
================ Snippet End ================<br>
================ Snippet Begin ================<br>
SVN checkout the contents of a folder, not the folder itself<br>
<br>
svn checkout file:///home/landonwinters/svn/waterproject/trunk .<br>
================ Snippet End ================<br>
================ Snippet Begin ================<br>
SVN not updating recursively<br>
<br>
svn up --depth infinity<br>
svn up --set-depth infinity<br>
================ Snippet End ================<br>
================ Snippet Begin ================<br>
start mySQL server from command line on Mac OS Lion<br>
<br>
shell> sudo /usr/local/mysql/bin/mysqld_safe<br>
(Enter your password, if necessary)<br>
(Press Control-Z)<br>
shell> bg<br>
(Press Control-D or enter "exit" to exit the shell)<br>
================ Snippet End ================<br>
================ Snippet Begin ================<br>
The following (save as rr.sh) reads a file from the command line:<br>
<br>
#!/bin/bash<br>
while read line<br>
do<br>
name=$line<br>
echo "Text read from file - $name"<br>
done < $1<br>
================ Snippet End ================<br>
================ Snippet Begin ================<br>
search a string in Shell script variable<br>
<br>
flag=`echo $a|awk '{print match($0,"pass")}'`;<br>
<br>
if [ $flag -gt 0 ];then<br>
<br>
echo "Success";<br>
else<br>
echo "fail";<br>
<br>
fi<br>
================ Snippet End ================<br>
================ Snippet Begin ================<br>
generate ssl cert & key for apache<br>
<br>
openssl genrsa 1024 >server.key<br>
openssl req -new -key server.key > server.csr<br>
openssl req -x509 -days 3650 -key server.key -in server.csr >server.crt<br>
================ Snippet End ================<br>
================ Snippet Begin ================<br>
Mysql CREATE TABLE,Primary key,insert record<br>
<br>
CREATE TABLE Persons<br>
(<br>
P_Id int NOT NULL,<br>
LastName varchar(255) NOT NULL,<br>
FirstName varchar(255),<br>
Address varchar(255),<br>
City varchar(255),<br>
PRIMARY KEY (P_Id)<br>
)<br>
<br>
INSERT INTO table_name (column1,column2,column3,...)<br>
VALUES (value1,value2,value3,...);<br>
================ Snippet End ================<br>
================ Snippet Begin ================<br>
Set / change / reset the MySQL root password on Ubuntu Linux. Enter the following lines in your terminal.<br>
<br>
1.Stop the MySQL Server.<br>
sudo /etc/init.d/mysql stop<br>
<br>
2.Start the mysqld configuration.<br>
sudo mysqld --skip-grant-tables &<br>
<br>
3.Login to MySQL as root.<br>
mysql -u root mysql<br>
<br>
4.Replace YOURNEWPASSWORD with your new password!<br>
UPDATE user SET Password=PASSWORD('YOURNEWPASSWORD') WHERE User='root'; FLUSH PRIVILEGES; exit;<br>
================ Snippet End ================<br>
================ Snippet Begin ================<br>
PHP Connect to the MySQL Server<br>
<br>
<?php<br>
// Create connection<br>
$con=mysqli_connect("example.com","peter","abc123","my_db");<br>
<br>
// Check connection<br>
if (mysqli_connect_errno()) {<br>
echo "Failed to connect to MySQL: " . mysqli_connect_error();<br>
}<br>
$result = mysqli_query($con,"SELECT 2015-03-25-Code-Snippet.md AutoBuilderClient.app BuilderHistory Repository addlebr.sh content errors.log iOS Advanced Topics.key iOS programming topics images out.txt 《杭州研究院-移动应用部-OSX应用组-王康-开发-技术-P3-2》.xls 资讯正文简析.key FROM Persons");<br>
<br>
while($row = mysqli_fetch_array($result)) {<br>
echo $row['FirstName'] . " " . $row['LastName'];<br>
echo "<br>";<br>
}<br>
<br>
mysqli_close($con);<br>
?><br>
================ Snippet End ================<br>
================ Snippet Begin ================<br>
SQL - IF EXISTS UPDATE ELSE INSERT INTO<br>
1.Create a UNIQUE constraint on your subs_email column, if one does not already exist:<br>
ALTER TABLE subs ADD UNIQUE (subs_email)<br>
<br>
2.Use INSERT ... ON DUPLICATE KEY UPDATE:<br>
INSERT INTO subs<br>
(subs_name, subs_email, subs_birthday)<br>
VALUES<br>
(?, ?, ?)<br>
ON DUPLICATE KEY UPDATE<br>
subs_name = VALUES(subs_name),<br>
subs_birthday = VALUES(subs_birthday)<br>
================ Snippet End ================<br>
================ Snippet Begin ================<br>
Reading HTML content from a UIWebView<br>
NSString *html = [yourWebView stringByEvaluatingJavaScriptFromString:<br>
@"document.body.innerHTML"];<br>
================ Snippet End ================<br>
================ Snippet Begin ================<br>
Cropping a UIImage<br>
CGImageRef imageRef = CGImageCreateWithImageInRect([largeImage CGImage], cropRect);<br>
// or use the UIImage wherever you like<br>
[UIImageView setImage:[UIImage imageWithCGImage:imageRef]];<br>
CGImageRelease(imageRef);<br>
================ Snippet End ================<br>
================ Snippet Begin ================<br>
How to change Status Bar text color in iOS 7.<br>
Set the UIViewControllerBasedStatusBarAppearance to YES in the .plist file.<br>
<br>
In the viewDidLoad do a [self setNeedsStatusBarAppearanceUpdate];<br>
<br>
Add the following method:<br>
<br>
-(UIStatusBarStyle)preferredStatusBarStyle{<br>
return UIStatusBarStyleLightContent;<br>
}<br>
================ Snippet End ================<br>
================ Snippet Begin ================<br>
makeObjectsPerformSelector:<br>
NSArray *newArray = [[NSArray alloc] initWithArray:oldArray copyItems:YES];<br>
[newArray makeObjectsPerformSelector:@selector(doSomethingToObject)];<br>
================ Snippet End ================<br>
================ Snippet Begin ================<br>
iOS - Identify what UIViewController was previously shown in a UINavigationController<br>
- (void)navigationController:(UINavigationController *)navigationController willShowViewController:(UIViewController *)viewController animated:(BOOL)animated<br>
================ Snippet End ================<br>
================ Snippet Begin ================<br>
Efficiently finding the average color of a UIImage<br>
<br>
#import "UIImage+AverageColor.h"<br>
<br>
@implementation UIImage (AverageColor)<br>
<br>
- (UIColor *)averageColor {<br>
<br>
CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();<br>
unsigned char rgba[4];<br>
CGContextRef context = CGBitmapContextCreate(rgba, 1, 1, 8, 4, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);<br>
<br>
CGContextDrawImage(context, CGRectMake(0, 0, 1, 1), self.CGImage);<br>
CGColorSpaceRelease(colorSpace);<br>
CGContextRelease(context);<br>
<br>
if(rgba[3] &gt; 0) {<br>
CGFloat alpha = ((CGFloat)rgba[3])/255.0;<br>
CGFloat multiplier = alpha/255.0;<br>
return [UIColor colorWithRed:((CGFloat)rgba[0])*multiplier<br>
green:((CGFloat)rgba[1])*multiplier<br>
blue:((CGFloat)rgba[2])*multiplier<br>
alpha:alpha];<br>
}<br>
else {<br>
return [UIColor colorWithRed:((CGFloat)rgba[0])/255.0<br>
green:((CGFloat)rgba[1])/255.0<br>
blue:((CGFloat)rgba[2])/255.0<br>
alpha:((CGFloat)rgba[3])/255.0];<br>
}<br>
}<br>
<br>
@end<br>
================ Snippet End ================<br>
================ Snippet Begin ================<br>
Linux provides a nice little command which makes our lives a lot easier.<br>
GET:<br>
with JSON:<br>
curl -i -H "Accept: application/json" -H "Content-Type: application/json" http://hostname/resource<br>
with XML:<br>
curl -H "Accept: application/xml" -H "Content-Type: application/xml" -X GET http://hostname/resource<br>
POST:<br>
For posting data:<br>
curl --data "param1=value1&param2=value2" http://hostname/resource<br>
For file upload:<br>
curl --form "fileupload=@filename.txt" http://hostname/resource<br>
RESTful HTTP Post:<br>
curl -X POST -d @filename http://hostname/resource<br>
For logging into a site (auth):<br>
curl -d "username=admin&password=admin&submit=Login" --dump-header headers http://localhost/Login<br>
curl -L -b headers http://localhost/<br>
================ Snippet End ================<br>
================ Snippet Begin ================<br>
lipo -info xx.a<br>
creates or operate on fat (multi-architecture) files.<br>
================ Snippet End ================<br>
================ Snippet Begin ================<br>
curl --header "Cookie: xq_a_token=tokenvalue;" http://api.xueqiu.com/v4/stock/quote.json?code=SHcode<br>
================ Snippet End ================<br>
