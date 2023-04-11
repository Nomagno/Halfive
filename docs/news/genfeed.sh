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

# Generate RSS feed from files in directory
# Requirements:
# POSIX sh
# POSIX sed
# POSIX grep
# POSIX ls
# POSIX printf

FEED_FILE='rss.xml'
TEMPLATE='<?xml version="1.0" encoding="US-ASCII" ?>
<rss version="2.0">
<channel>
<title>Halfive Blog</title>
<description>Feed for news on the development of Halfive</description>
<link>_URL/index.html</link>
<copyright>2021,2022 Nomagno MIT License</copyright>'
ITEM='<item>
<title>_TITLE</title>
<description>Halfive Blog Entry</description>
<link>_URL/news/_FILE</link>
</item>
'
CLOSURE='</channel></rss>'
REP_URL='http://halfive.nomagno.xyz'

printf '%s\n' "$TEMPLATE" | sed "s|_URL|$REP_URL|g" > $FEED_FILE
for REP_FILE in $(ls | grep 'html' | grep -ve 'genfeed.sh' -ve "$FEED_FILE"); do
REP_TITLE=$(printf '%s\n' "$REP_FILE" | sed 's|_| |g; s|\..*$||g')
printf '\n%s\n' "$ITEM" | sed "s|_URL|$REP_URL|g; s|_FILE|$REP_FILE|g; s|_TITLE|$REP_TITLE|g" \
>> $FEED_FILE
done
printf '%s' "$CLOSURE" >> $FEED_FILE
