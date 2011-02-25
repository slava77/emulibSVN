# .bashrc

# User specific aliases and functions

# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi
alias xemacs='nedit'
#. ~cscpro/bin/getCSCHostname.sh
#PS1="\u@$CSCHOSTNAME ${PWD/#$HOME/~}>"
#PROMPT_COMMAND='echo -ne
#"\033]0;${USER}@${CSCHOSTNAME}:${PWD/#$HOME/~}\007"'
