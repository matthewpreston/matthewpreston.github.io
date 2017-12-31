#!/bin/bash
#Used to see if load is above maximum threshold

emailFile="/path/to/file/email.txt"

echo "Please open this script in notepad and replace the /path/to/file to a file"
echo "which contains an email for each line. Then you can remove these echo"
echo "statements and the following exit statement."
exit 0

MAXLOAD=$(cat /proc/cpuinfo | grep -c processor)
current=$(cat /proc/loadavg | awk '{print $1}')

bool=$(echo $current'>'$MAXLOAD | bc -l)
if [ $bool -eq 1 ]; then
  emails=($(cat $emailFile))
  output="temp.txt"
  echo -e "Load is greater than maximum!\n\n" > $output
  echo -e "Current: $current\nMaximum: $MAXLOAD\n\n" >> $output
  echo "Please elucidate the problem at hand" >> $output
  for email in ${emails[@]}; do
    cat $output | mail -s "Server Load Over Maximum" $email
  done
fi
