# TDD in iOS

![TDD icon](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/_images/tdd.jpg)

## System test

Automated with iOS apps - **UI Automation**

### Penetration Test
Look for security problems by feeding the application with malformed input.

#### Usability test
Users interacting with the application,taking note of anything that the users got wrong.

#### Unit Test
Unit tests are small pieces of code that test the behavior of other code. They set up the preconditions,run the code under test, and make assertions about the final state.

#### Repeatable tests
Warn you about regression bugs. Provide a safety net where you want to edit the source without any change in behavior.

#### Code coverage
Measuring code coverage partially addresses the problems with test counting by mesuring the account of application code that is being executed when the tests are run. They may have value, but they still aren't the most value of your time.Eg, switch statements and or cases.

#### Red,Green,Refactor
It's all very well saying that you should write the best before you write the code,but how do you write that test?
It I had to use code that should solved this problem, how would i want to use it?
Red -> Something wrong -> Write code to make it work -> Green -> Eliminate Redundancy (Refactor,don't hurry to do that).

Red -> Green -> Refactor -> Red(Iterator)
vs 
A dozen features that have all been started but are all incomplete and unusable.

#### Refactor

Turn bad smelled codes into codes without smell.

#### YAGNI
Ya ain't gonna need it.

#### Unit Test Requirement
Identify what the application needs to do.

#### Unit Test Case Design
Design aimming at interfaces.
Write basic test cases(interfaces),modify it to meet all demands(implements),move test code to real code, refactoring, test the combined one.
Fake objects instead of more complicated objects.

#### Making the tests more readable
Macro magics

#### Frameworks
STTest\GHUnit\OCMock

#### CI
Hudson,Jenkins,Xcode Server

#### Data Model
init
property
interfaces
unit -> combine

#### Application Logic
Web Request -> State 
Error Definition -> Error
Exception Handling -> Exception
Interface Test

#### Networking Code
Status
URL
Fake Responses

#### View Controllers
viewDidLoad
viewDidAppear
viewDidDisappear,etc
runtime apis

#### Design for TDD
*Design to interfaces,not implementations.*
<Makes it easy to replace dependencies on complex systems with fack or mock objects>.
*Tell,Don't Ask.*
Procedural code gets information then makes decisions. Object-oriented code tells objects to do things. 
	- Alex Sharp
*High cohesion and low coupling*
*Encapsulation*
*Don't be cleverer than necessary*
*Prefer a wide,shallow inheritance hierachy*

#### Beyond today's TDD
*Expressing ranges of input and output*
*BDD*

## 更多
Contact [KyleWong](mailto:kang.wang1988@gmail.com) for more.