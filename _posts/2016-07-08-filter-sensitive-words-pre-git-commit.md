---
layout: post
comments: true
section-type: post
title: 如何使用git钩子过滤敏感内容
category: tech
tags: [ 'tutorial' ]
---
### ###引言

    如何检查你的git提交内容是否包含敏感信息？如何设置默认的git提交消息？如何验证客户端的内容是否具有良好的风格？怎么在服务端强制客户端提交符合格式的git记录？如何在git的各个阶段通知相关人员？如何针对不同的仓库使用自定义的敏感词过滤？

    这些问题的答案就在于git提供的hook功能。

## ###git钩子

​	git hook(钩子)是一种方便用户在git的各阶段执行自定义操作的机制，默认情况下，所有的hook都存储在git仓库的.git/hooks文件夹里。

服务端支持的钩子包括:

>	pre-receive:当服务器处理来自客户端的push请求时。
>	update:当每个分支上，pusher意图更新内容时。
>	post-receive:当整个receive过程结束可用来update时。

客户端支持的钩子包括:

>	pre-commit:在键入commit消息之前执行，返回非零可以中止commit。可以在此阶段执行代码检查等工作，git commit --no-verify可以越过pre-commit钩子。
>	prepare-commit-msg:在commit message编辑器被唤醒后，但默认消息创建前执行。可结合commit 模板在此阶段编程插入信息。
>	commit-msg:传入一个包含提交信息的临时文件路径作为参数，可在此验证工程状态，提交信息等，返回非零值将中止提交过程。
>	post-commit:当整个commit过程结束后执行。通常用来做类似通知的事情。
>	pre-rebase:在rebase之前执行，可通过返回非零值中断此过程。
>	post-rewrite:当执行替换commit如(git commit --amend或git rebase)的时候执行。
>	post-checkout:当成功执行git checkout之后执行，可在此阶段配置正确的项目开发环境。
>	post-merge:当成功执行merge之后执行，可使用它恢复权限数据等git无法追踪的工作树数据。
>	pre-push:当远端已经updated，但在所有对象被传输之前执行。可使用它来验证更新引用，返回非零可中断push过程。
>	pre-auto-gc: git会时不时在操作中调用git gc --auto执行垃圾收集操作，在垃圾收集之前pre-auto-gc钩子被执行。

此外，对于基于email的git工作流，当git am处理patch时，以下钩子将被调用:

>	pre-applypatch:传入参数是一个指向包含了commit消息的临时文件，可在此阶段确保提交消息被正确格式化，返回非零值中止patch过程。
>	applypatch-msg:当patch应用后，但未commit之前。返回非零将终止git am脚本且不提交此次patch。
>	post-applypatch: commit之后执行。

### ###敏感词过滤

​	下面就以敏感词过滤为例说明如何正确地使用git的pre-commit完成用户所需功能。

>1. git提供了一组后缀为.sample的钩子，将其pre-commit.sample重命名为pre-commit至于仓		库.git/hooks下即可保证pre-commit在提交前被执行。
>2. git config --global配置全局和不同仓库的敏感词列表。
>3. 拷贝(如无则创建)git-template/hooks文件夹，并设置为init.templatedir,这样每次用户执行git init(任意一次)时，都会将其git-templates目录下的内容全部拷贝到当前仓库的.git目录下。
>4. 编辑pre-commit文件，提取提交内容并判断是否包含了已配置的敏感词列表，如果包含，错误提醒用户并终止commit。

### ###源代码

[commit-guard.sh](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/others/commit-guard.sh)

[pre-commit](https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/others/pre-commit)

​	下载并将上述两个文件置于同目录下后，根据需要修改sh中的敏感词字段，再执行commit-guard.sh即可。

### ###参考资料

[Customizing Git Git-Hooks](https://git-scm.com/book/en/v2/Customizing-Git-Git-Hooks)
​	