import sys
from Bio import SeqIO
from Bio.Alphabet import generic_dna
from Bio.Seq import Seq
from Bio.SeqRecord import SeqRecord

outputDir = sys.argv[1]
records = {}
for arg in sys.argv[2:]:
    fastaRecords = SeqIO.parse(arg, "fasta")
    for record in fastaRecords:
        if records.get(record.id):
            records[record.id].append(record)
        else:
            records[record.id] = [record]
sortedKeys = records.keys()
sortedKeys.sort(key=(lambda x: x.lower()))
for key in sortedKeys:
    SeqIO.write(records[key], "%s/%s.fasta" % (outputDir,key), "fasta")