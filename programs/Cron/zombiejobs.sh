#!/bin/bash
#Used to see if jobs have been running for longer than $MAXTIME

USAGE="bash $0 [MAXTIME]"
emailFile="/path/to/file/email.txt"
jobsFile="/path/to/file/jobs.txt"

echo
echo "Please open this script in notepad and replace the /path/to/file to a file"
echo "which contains an email for each line. Do this as well for the jobs file,"
echo "but specify the job name as seen in the command 'top'. Then you can remove"
echo "these echo statements and the following exit statement."
exit 0

MAXTIME=$((60*60*24*7)) #Number of seconds in a week
if [ $# -gt 0 ]; then
  MAXTIME=$1
fi

#Read log file of problematic jobs that were running when last checked
logFile="log.txt"
if [ ! -f $logFile ]; then
  echo "PID Time_Elapsed" > $logFile
fi
logJobs=($(tail -n +2 $logFile))
logPID=()
logElapsed=()
for ((i=0; i < ${#logJobs[@]}; i+=2)); do
  logPID+=(${logJobs[i]})
  logElapsed+=(${logJobs[i+1]})
done

#Get current time in seconds since Unix Epoch
currentTime=$(date +"%s")

#Get all jobs to look out for
jobsArray=($(cat $jobsFile))
[ ${#jobsArray[@]} -eq 0 ] && exit 0
jobStr="${jobsArray[0]}"
for elem in ${jobsArray[@]:1}; do
  jobStr="$jobStr|$elem"
done

#Get PIDs of all jobs
JobsPID=($(ps -e | grep -E $jobStr | awk '{printf "%s ",$1}'))
problemPID=()
problemElapsed=()
emailProblems=()
for pid in ${JobsPID[@]}; do
  jobStart=$(stat -c%Z /proc/$pid/stat)
  if (( ($currentTime - $jobStart) > $MAXTIME )); then
    problemPID+=($pid)
    problemElapsed+=($(($currentTime - $jobStart)))
    #Job has exceeded maximum length, see if this has been recorded
    recorded=0
	for lpid in ${logPID[@]}; do
	  if [ $pid -eq $lpid ]; then
	    recorded=1
		break
	  fi
	done
	#Only email about new problems
	if [ $recorded -eq 0 ]; then
      emailProblems+=($pid)
	fi
  fi
done

#A job has ran longer than a week (oh noes)
if [ ${#emailProblems[@]} -gt 0 ]; then
  emails=($(cat $emailFile))
  output="temp.txt"
  echo "List of PIDs of jobs that have ran longer than expected ($MAXTIME seconds):" > $output
  echo >> $output
  for entry in ${emailProblems[@]}; do
    echo $entry >> $output
  done
  echo >> $output
  echo "Please ensure that these jobs are dealt with appropriately." >> $output
  for email in ${emails[@]}; do
    cat $output | mail -s "Zombie Jobs" $email
  done
fi

#Update log file
output="log.txt"
echo "PID Time_Elapsed" > $output
for ((i=0; i < ${#problemPID[@]}; i++)); do
  echo "${problemPID[i]} ${problemElapsed[i]}" >> $output
done

