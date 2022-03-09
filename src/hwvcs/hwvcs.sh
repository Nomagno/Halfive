#!/bin/sh

#Copyright Nomagno 2022, 2022

#Redistributions, modified or unmodified, in whole or in part, must retain
#applicable notices of copyright or other legal privilege, these conditions, and
#the following license terms and disclaimer.  Subject to these conditions, each
#holder of copyright or other legal privileges, author or assembler, and
#contributor of this work, henceforth "licensor", hereby grants to any person
#who obtains a copy of this work in any form:

#1. Permission to reproduce, modify, distribute, publish, sell, sublicense, use,
#and/or otherwise deal in the licensed material without restriction.

#2. A perpetual, worldwide, non-exclusive, royalty-free, gratis, irrevocable
#patent license to make, have made, provide, transfer, import, use, and/or
#otherwise deal in the licensed material without restriction, for any and all
#patents held by such licensor and necessarily infringed by the form of the work
#upon distribution of that licensor's contribution to the work under the terms
#of this license.

#NO WARRANTY OF ANY KIND IS IMPLIED BY, OR SHOULD BE INFERRED FROM, THIS LICENSE
#OR THE ACT OF DISTRIBUTION UNDER THE TERMS OF THIS LICENSE, INCLUDING BUT NOT
#LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
#AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS, ASSEMBLERS, OR HOLDERS OF
#COPYRIGHT OR OTHER LEGAL PRIVILEGE BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER
#LIABILITY, WHETHER IN ACTION OF CONTRACT, TORT, OR OTHERWISE ARISING FROM, OUT
#OF, OR IN CONNECTION WITH THE WORK OR THE USE OF OR OTHER DEALINGS IN THE
#WORK.

# Requirements:
# POSIX compliant shell


NOBACKUP_ARGS='-V none'

applycommit() {
	mkdir "$1"
	CURR="$PWD"
	COUNT=0
	cd "$1"
	while [ "$(echo "$COMPARE" | head -c 1)" != '-' ]; do
		patch $NOBACKUP_ARGS -us -p0 < "$CURR/.hwvcs/commits/${COUNT}.patch"
		COUNT=$((COUNT + 1))
		COMPARE=$(("$2" - COUNT))
	done
	cd "$CURR"

}

HELPMESSAGE='Usage:
hwvcs.sh [OPTION] [ARG]
-n   New project, ARG - path of info file to copy
-c   Create new commit, ARG - path of info file to copy
-r   Rollback to commit, ARG - index
-i   Show info about commit, ARG - either "REPO" or the index
-h   Print this message
'

if [ "$1" = '-n' ]; then
	mkdir -p .hwvcs/commits
	cp "$2" .hwvcs/info
	mkdir tree
elif [ "$1" = '-c' ]; then
	LASTCOMMIT=$(head -n 2 .hwvcs/info | tail -n 1)
	if [ "$LASTCOMMIT" != 'NONE' ]; then
		mv tree treebackup
		mkdir tree.base
		applycommit tree "$LASTCOMMIT"
		rm -r tree.base
		mv tree tree.base
		mv treebackup tree
		ls tree tree.base
		NEWCOMMIT=$((LASTCOMMIT + 1))
	else
		mkdir tree.base
		NEWCOMMIT=0
	fi
	cd tree
	diff -Nru ../tree.base . > "../.hwvcs/commits/${NEWCOMMIT}.patch"
	cd ..
	rm -rf tree.base
	cp "$2" ".hwvcs/commits/${NEWCOMMIT}.info"
	(echo 2c; echo $NEWCOMMIT; echo .; echo w) | ed - .hwvcs/info
elif [ "$1" = '-r' ]; then
	mkdir tree.base
	rm -rf tree
	applycommit tree "$2"
	rm -rf tree.base
elif [ "$1" = '-i' ]; then
	if [ "$2" = 'REPO' ]; then
		echo 'Repo information:'
		cat .hwvcs/info
	else
		echo "Information for commit ${2}:"
		cat ".hwvcs/commits/${2}.info"
	fi
else
	printf '%s' "$HELPMESSAGE"
fi
