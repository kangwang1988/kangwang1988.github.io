# 使用Xcode、OS X Server、XCTest、Git持续集成

![Continous Intergration](http://blog.jki.net/wp-content/uploads/2012/09/jki-continuous-integration-graphic-091812.png)

## 何为持续集成

在软件工程实践中，通过把所有人的工作副本集合起来进行一次或多次构建的过程成为**持续集成(Continuous integration)**。持续集成被作为极限编程的一部分引入实践，其主要是为了防止在早期极限编程中提到的集成的问题.早期是用来在测试驱动开发的实践中用于自动化测试的，通过在开发者本地机器中运行所有的单元测试，验证其均通过测试以提交到主干，这样就避免了一个开发者进行中的工作打扰到另一个开发者。后来CI这一概念引入了构建服务器，其通过周期性地运行测试用例或者在提交后讲结果报告给开发者。除过单元测试外，使用持续集成的组织通常使用构建服务器来实现质量控制-每次一点，频繁使用.除过单元和集成测试外，还包括额外的静态和动态测试,衡量和分析性能，从源代码中抽取和形成文档，以及帮助手动QA过程。持续的质量控制旨在改善软件质量，减少分发时间，用于替换原有的开发结束后的质量控制，这也通传统的频繁集成以简化集成理念一致。
同样地，持续集成拓展出的持续发布也确保了主干上的软件随时处于克发布的状态，使得实际部署的过程很快。

### iOS中的持续集成
![Xcode continous integration workflow](https://developer.apple.com/library/ios/documentation/IDEs/Conceptual/xcode_guide-continuous_integration/art/continuous_integration_2x.png)

此框架中，OSX Server(原本是一套独立的系统，从Lion开始作为一个工具包存在)充当服务器角色，从托管库中获取代码并完成分析、构建、测试、打包等持续集成过程，支持主流的Git/SVN代码管理。开发者通过在Developer Machine中完成代码和测试用例编写后，提交给托管库，通过设置Bot,可在每次提交后，由服务器自动运行持续集成的过程，并将构建结果通知用户。

### 环境搭建
1.下载并安装OSX Server

[点击前往Mac Store下载](https://itunes.apple.com/us/app/os-x-server/id883878097?mt=12)

$19.9刀感觉贵的话，可以去百度云盘搜索。

OSX Server下载完成后，打开并安装，选中你需要管理的机器并登陆。
在左侧服务中选择Xcode服务并打开使能，右侧控制面板中选中用于持续集成的Xcode.app，也可以填写开发着团队,添加设备等。
Xcode中的储存库此处可不填写，在Xcode中创建Repository的时候选择在服务器创建即可。

2.Xcode创建工程和Bot

* Xcode中创建工程，最后一步Source Control选择Create Git repository on <your osx server hostname/ip address>.

* 如果没有在创建工程的时候指定，也可以通过Source Control -> Working Copy ->Configure->Remote中的添加按钮去添加repository.

* Project->Create Bot，根据Wizard的提示，逐步完成Bot设置。需要注意的是，Server应选中OS X Server所对应的Server.如果最开始选中了Integrate immediately将在Bot创建完成后开始集成。Test with选中设备时，默认的All iOS Devices and Simulators会使得系统针对所有已经安装的Simulators去构建。当设备上安装了过多Simulators的时候，这一步骤将很慢，因此选中那些自己需要构建的平台即可。After Intergration的时候，可以选择Notify Committers以接收集成结果的通知邮件。

3.运行持续集成

* 如果创建Bot的时候选中了On Commit，那么提交代码即会运行Bot，否则将按照创建时的设置区运行Bot。
* Bot的运行主要包括了:Preparing-Checkout-Build-Test-Archive-处理结果-Upload等。
* 
**strong** or __strong__ ( Cmd + B )

*emphasize* or _emphasize_ ( Cmd + I )

**Sometimes I want a lot of text to be bold.
Like, seriously, a _LOT_ of text**

#### Blockquotes

> Right angle brackets &gt; are used for block quotes.

#### Links and Email

An email <example@example.com> link.

Simple inline link <http://chenluois.com>, another inline link [Smaller](http://25.io/smaller/), one more inline link with title [Resize](http://resizesafari.com "a Safari extension").

A [reference style][id] link. Input id, then anywhere in the doc, define the link with corresponding id:

[id]: http://25.io/mou/ "Markdown editor on Mac OS X"

Titles ( or called tool tips ) in the links are optional.

#### Images

An inline image ![Smaller icon](http://25.io/smaller/favicon.ico "Title here"), title is optional.

A ![Resize icon][2] reference style image.

[2]: http://resizesafari.com/favicon.ico "Title"

#### Inline code and Block code

Inline code are surround by `backtick` key. To create a block code:

	Indent each line by at least 1 tab, or 4 spaces.
    var Mou = exactlyTheAppIwant; 

####  Ordered Lists

Ordered lists are created using "1." + Space:

1. Ordered list item
2. Ordered list item
3. Ordered list item

#### Unordered Lists

Unordered list are created using "*" + Space:

* Unordered list item
* Unordered list item
* Unordered list item 

Or using "-" + Space:

- Unordered list item
- Unordered list item
- Unordered list item

#### Hard Linebreak

End a line with two or more spaces will create a hard linebreak, called `<br />` in HTML. ( Control + Return )  
Above line ended with 2 spaces.

#### Horizontal Rules

Three or more asterisks or dashes:

***

---

- - - -

#### Headers

Setext-style:

This is H1
==========

This is H2
----------

atx-style:

# This is H1
## This is H2
### This is H3
#### This is H4
##### This is H5
###### This is H6


### Extra Syntax

#### Footnotes

Footnotes work mostly like reference-style links. A footnote is made of two things: a marker in the text that will become a superscript number; a footnote definition that will be placed in a list of footnotes at the end of the document. A footnote looks like this:

That's some text with a footnote.[^1]

[^1]: And that's the footnote.


#### Strikethrough

Wrap with 2 tilde characters:

~~Strikethrough~~


#### Fenced Code Blocks

Start with a line containing 3 or more backticks, and ends with the first line with the same number of backticks:

```
Fenced code blocks are like Stardard Markdown’s regular code
blocks, except that they’re not indented and instead rely on
a start and end fence lines to delimit the code block.
```

#### Tables

A simple table looks like this:

First Header | Second Header | Third Header
------------ | ------------- | ------------
Content Cell | Content Cell  | Content Cell
Content Cell | Content Cell  | Content Cell

If you wish, you can add a leading and tailing pipe to each line of the table:

| First Header | Second Header | Third Header |
| ------------ | ------------- | ------------ |
| Content Cell | Content Cell  | Content Cell |
| Content Cell | Content Cell  | Content Cell |

Specify alignment for each column by adding colons to separator lines:

First Header | Second Header | Third Header
:----------- | :-----------: | -----------:
Left         | Center        | Right
Left         | Center        | Right


### Shortcuts

#### View

* Toggle live preview: Shift + Cmd + I
* Toggle Words Counter: Shift + Cmd + W
* Toggle Transparent: Shift + Cmd + T
* Toggle Floating: Shift + Cmd + F
* Left/Right = 1/1: Cmd + 0
* Left/Right = 3/1: Cmd + +
* Left/Right = 1/3: Cmd + -
* Toggle Writing orientation: Cmd + L
* Toggle fullscreen: Control + Cmd + F

#### Actions

* Copy HTML: Option + Cmd + C
* Strong: Select text, Cmd + B
* Emphasize: Select text, Cmd + I
* Inline Code: Select text, Cmd + K
* Strikethrough: Select text, Cmd + U
* Link: Select text, Control + Shift + L
* Image: Select text, Control + Shift + I
* Select Word: Control + Option + W
* Select Line: Shift + Cmd + L
* Select All: Cmd + A
* Deselect All: Cmd + D
* Convert to Uppercase: Select text, Control + U
* Convert to Lowercase: Select text, Control + Shift + U
* Convert to Titlecase: Select text, Control + Option + U
* Convert to List: Select lines, Control + L
* Convert to Blockquote: Select lines, Control + Q
* Convert to H1: Cmd + 1
* Convert to H2: Cmd + 2
* Convert to H3: Cmd + 3
* Convert to H4: Cmd + 4
* Convert to H5: Cmd + 5
* Convert to H6: Cmd + 6
* Convert Spaces to Tabs: Control + [
* Convert Tabs to Spaces: Control + ]
* Insert Current Date: Control + Shift + 1
* Insert Current Time: Control + Shift + 2
* Insert entity <: Control + Shift + ,
* Insert entity >: Control + Shift + .
* Insert entity &: Control + Shift + 7
* Insert entity Space: Control + Shift + Space
* Insert Scriptogr.am Header: Control + Shift + G
* Shift Line Left: Select lines, Cmd + [
* Shift Line Right: Select lines, Cmd + ]
* New Line: Cmd + Return
* Comment: Cmd + /
* Hard Linebreak: Control + Return

#### Edit

* Auto complete current word: Esc
* Find: Cmd + F
* Close find bar: Esc

#### Post

* Post on Scriptogr.am: Control + Shift + S
* Post on Tumblr: Control + Shift + T

#### Export

* Export HTML: Option + Cmd + E
* Export PDF:  Option + Cmd + P


### And more?

Don't forget to check Preferences, lots of useful options are there.

Follow [@Mou](https://twitter.com/mou) on Twitter for the latest news.

For feedback, use the menu `Help` - `Send Feedback`