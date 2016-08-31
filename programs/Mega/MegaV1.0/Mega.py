import getopt
import sys
from Bio import SeqIO
from Mega.TerminalCommands import *
from Mega.OutputMode import OutputMode
import xlsxwriter

USAGE = """\n Usage: %s [options] <aln.fasta> [aln2.fasta...]

WORD OF WARNING: Large batch sizes may bloat the Excel file and cause computer
crashes (from experience btw). It has been limited to a maximum of 20 (untested)

Parameters:
    aln.fasta       File which contains all sequences which align to a gene.
                    Preferably make the first entry be the "reference"

Options:
    -b INT, --batch=INT        Takes INT files to condense into one [10]
    -m STR, --modes=STR        Any combination of the following: [dp]
                                    "i"        Individual alignment to ref
                                    "d"        Align all DNA sequences
                                    "p"        Align translated DNA sequences
    -o TITLE, --output=TITLE   Title of desired output file (I will put .xlsx)
                               ["MEGA_" + TIME_RUN]
""" % sys.argv[0]

MAX_BATCH_SIZE = 20

def main(argv):
    opt_str = "hb:m:o:"
    opt_list = ["help", "batch=", "modes=", "output="]
    try:
        opts, args = getopt.getopt(argv, opt_str, opt_list)
    except getopt.GetoptError as err:
        FPrint(sys.stderr, str(err))
        FPrint(sys.stderr, USAGE)
        sys.exit(2)
    fastaFiles = []
    batch = 10
    modes = ['d','p']
    outTitle = None
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            print(USAGE)
            sys.exit()
        elif opt in ("-b", "--batch"):
            batch = int(arg)
        elif opt in ("-m", "--modes"):
            modes = list(arg)
        elif opt in ("-o", "--output"):
            outTitle = arg
    #Get input/output files
    fastaFiles = args
    for f in fastaFiles:
        CheckParam(f, CheckFiles, fail_str="Invalid FASTA file: %s", exit=True)
        CheckParam(batch, lambda x: x>0 and x<=MAX_BATCH_SIZE,
                   fail_str="Invalid batch size: %d (0 < x <= 20)", exit=True)
    if not outTitle:
        outTitle = "Mega_" + GetDate()
    
    #Retrieve sequences
    records = {}
    for f in fastaFiles:
        for record in SeqIO.parse(f, "fasta"):
            if records.get(record.id):
                records[record.id].append(record)
            else:
                records[record.id] = [record]
    
    #Create workbook, write, and close
    sortedKeys = records.keys()
    sortedKeys.sort(key=(lambda x: x.lower()))
    for count in range(0, len(sortedKeys), batch):
        if batch == 1:
            outFile = outTitle + ".xlsx"
        else:
            start = sortedKeys[count]
            end = sortedKeys[min(count+batch,len(sortedKeys)) - 1]
            outFile = outTitle + "_" + start + "_to_" + end + ".xlsx"
        workbook = xlsxwriter.Workbook(outFile)
        seqRecords = [records[key] for key in sortedKeys[count:count+batch]]
        OutputMode(workbook, modes, seqRecords)
        workbook.close()
            
if __name__ == "__main__":
    main(sys.argv[1:])
