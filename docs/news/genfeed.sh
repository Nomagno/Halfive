FEED_FILE='rss.xml'
TEMPLATE='<?xml version="1.0" encoding="US-ASCII" ?>
<rss version="2.0">
<channel>
<title>Half-World Blog</title><description>Feed for news on the development of Half-World</description><link>_URL/index.html</link><copyright>2021,2022 Nomagno COIL License</copyright>'
ITEM='<item><title>_TITLE</title><description>Half-World Blog Entry</description><link>_URL/news/_FILE</link></item>'
CLOSURE='</channel></rss>'
REP_URL='http://halfworld.nomagno.xyz'

printf '%s\n' "$TEMPLATE" | sed "s|_URL|$REP_URL|g" > $FEED_FILE
for REP_FILE in $(ls | grep 'html' | grep -ve 'genfeed.sh' -ve "$FEED_FILE"); do
REP_TITLE=$(echo $REP_FILE | sed 's|_| |g; s|\..*$||g')
printf '%s\n' "$ITEM" | sed "s|_URL|$REP_URL|g; s|_FILE|$REP_FILE|g; s|_TITLE|$REP_TITLE|g" >> $FEED_FILE
done
printf '%s' "$CLOSURE" >> $FEED_FILE
