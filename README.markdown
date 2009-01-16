# Global setup
* Download and install [Git](http://git-scm.com/download)
	
    git config --global user.email yourgit@email.net

# Next step
* mkdir mrpara
* cd mrpara
* git init
* touch README
* git add README
* git commit -m 'first commit'
* git remote add origin git@github.com:jk/mrpara.git
* git push origin master

# Exisiting Git Repo?
* cd existing_git_repo
* git remote add origin git@github.com:jk/mrpara.git
* git push origin master

# Importing a SVN Repo?
[Click here](http://github.com/jk/mrpara/imports/new)

# When you're done
[Continue](http://github.com/jk/mrpara/tree/master)

# GitHub specific
Look over the [Guidesection](http://github.com/guides/Home). You should also customize your ~/.gitconfig .

# .gitconfig

    [user]
    	name = Jens Kohl
    	email = jens.kohl@gmail.com
    	signingkey = 8EFDF903
    [color]
    	status = auto
    	diff = auto
    	branch = auto
    	interactive = auto
    [alias]
    	st = status
    	ci = commit
    	co = checkout
    	l = log
    	staged = diff --cached
    	unstaged = diff
    	both = diff HEAD
    	oneline = log --pretty=oneline
    	amend = commit --amend
    [merge]
    	tool = opendiff
    [sendemail]
    	smtpserver = example.com
    	aliasesfile = /Users/jkohl/.gitaliases
    	aliasfiletype = mailrc
    	smtpuser = user
    	smtppass = geheim
    	smtpssl = true
    [format]
    	numbered = auto
    [core]
    	legacyheaders = false
    	excludesfile = /etc/gitexcludes
    	editor = mate -w
    [repack]
    	usedeltabaseoffset = true
    [github]
    	user = jk
    	token = xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

Die Daten für die github-Direktive gibt es unter [Accounts](https://github.com/account).