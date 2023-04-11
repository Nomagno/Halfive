#!/bin/sh

#Copyright Nomagno 2021, 2022, 2023
#
#Permission is hereby granted, free of charge, to any person obtaining
#a copy of this software and associated documentation files (the "Software"),
#to deal in the Software without restriction, including without limitation
#the rights to use, copy, modify, merge, publish, distribute, sublicense,
#and/or sell copies of the Software, and to permit persons to whom the
#Software is furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice (including the next
#paragraph) shall be included in all copies or substantial portions of the
#Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
#IN THE SOFTWARE.

# Requirements:
# POSIX compliant environment

if patch --help >/dev/null 2>&1; then
	NOBACKUP_ARGS='--no-backup-if-mismatch'
else
	NOBACKUP_ARGS='-V none'
fi

bin_to_h5vcsbin(){
	LIST=$(find . -not -type d)
	for i in $LIST; do
		if file "$i" | grep -v -q ASCII; then
			od -An -tx1 -v "$i" | \
			awk '{$1=$1};1' | \
			tr ' ' '\n' > "${i}.h5vcs_bin"
			rm "$i"
		fi
	done
}

h5vcsbin_to_bin(){
	LIST=$(find . -not -type d)
	for i in $LIST; do
		if file "$i" | grep -q 'h5vcs_bin'; then
			while read -r line; do
				printf "\\$(printf %o 0x$line)"
			done < "$i" > "$(echo "$i" | sed 's/.h5vcs_bin//g')"
			rm "$i"
		fi
	done
}


applycommit() {
	mkdir "$1"
	CURR="$PWD"
	COUNT=0
	cd "$1"
	while [ "$(echo "$COMPARE" | head -c 1)" != '-' ]; do
		patch $NOBACKUP_ARGS -us -p0 -i "$CURR/.h5vcs/commits/${COUNT}.patch"
		COUNT=$((COUNT + 1))
		COMPARE=$(($2 - COUNT))
	done
	h5vcsbin_to_bin
	cd "$CURR"

}

HELPMESSAGE='USAGE: h5vcs.sh [OPTION] [ARG]
  -n   New project, ARG - path of info file to copy
  -c   Create new commit, ARG - path of info file to copy
  -r   Rollback to commit, ARG - index
  -i   Show info about commit, ARG - either "REPO" or the index
  -h   Print this message
'

if [ "$1" = '-n' ]; then
	mkdir -p .h5vcs/commits
	cp "$2" .h5vcs/info
	mkdir tree
elif [ "$1" = '-c' ]; then
	LASTCOMMIT=$(head -n 2 .h5vcs/info | tail -n 1)
	if [ "$LASTCOMMIT" != 'NONE' ]; then
		mv tree treebackup
		mkdir tree.base
		applycommit tree "$LASTCOMMIT"
		rm -r tree.base
		mv tree tree.base
		mv treebackup tree
		NEWCOMMIT=$((LASTCOMMIT + 1))
	else
		mkdir tree.base
		NEWCOMMIT=0
	fi
	cd tree
	bin_to_h5vcsbin
	diff -Nru ../tree.base . > "../.h5vcs/commits/${NEWCOMMIT}.patch"
	h5vcsbin_to_bin
	cd ..
	rm -rf tree.base
	cp "$2" ".h5vcs/commits/${NEWCOMMIT}.info"
	(echo 2c; echo $NEWCOMMIT; echo .; echo w) | ed - .h5vcs/info
elif [ "$1" = '-r' ]; then
	mkdir tree.base
	rm -rf tree
	applycommit tree "$2"
	rm -rf tree.base
elif [ "$1" = '-i' ]; then
	if [ "$2" = 'REPO' ]; then
		echo 'Repo information:'
		cat .h5vcs/info
	else
		echo "Information for commit ${2}:"
		cat ".h5vcs/commits/${2}.info"
	fi
else
	printf '%s' "$HELPMESSAGE"
fi
