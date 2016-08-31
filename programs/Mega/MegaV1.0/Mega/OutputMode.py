from math import ceil
from Translation import *

DNAColors = {
             "-":"#C0C0C0",
             "A":"#00FF00",
             "C":"#157DEC",
             "G":"#A74AC7",
             "T":"#FF0000",
             "K":"#C00040",
             "M":"#008080",
             "R":"#408000",
             "Y":"#008000",
             "S":"#4000C0",
             "W":"#808000",
             "B":"#FF6600",
             "V":"#00FFFF",
             "H":"#FFFF00",
             "D":"#FF00FF",
             "N":"#C0C0C0"
}

AAColors = {
            "*":"#C0C0C0",
            "-":"#C0C0C0",
            "?":"#C0C0C0",
            "A":"#FFFF00",
            "C":"#808000",
            "D":"#FF0000",
            "E":"#FF0000",
            "F":"#FFFF00",
            "G":"#FF00FF",
            "H":"#2B547E",
            "I":"#FFFF00",
            "K":"#157DEC",
            "L":"#FFFF00",
            "M":"#FFFF00",
            "N":"#4CC552",
            "P":"#FF8814",
            "Q":"#4CC552",
            "R":"#157DEC",
            "S":"#4CC552",
            "T":"#4CC552",
            "V":"#FFFF00",
            "W":"#4CC552",
            "Y":"#59E817",
            "X":"#C0C0C0"
}

def DNAFormat(workbook, nt):
    """Creates Format class for DNA nucleotides"""
    cellFormat = {"bg_color": DNAColors[nt],
                  "border": 1,
                  "border_color": "#FFFFFF",
                  "align": "center",
                  "valign": "center"}
    return workbook.add_format(cellFormat)
    
def AAFormat(workbook, aa, multi=False):
    """Creates Format class for amino acids"""
    cellFormat = {"bg_color": AAColors[aa],
                  "border": 1,
                  "border_color": "#FFFFFF",
                  "align": "center",
                  "valign": "center"}
    if multi:
        cellFormat["bg_color"] = "#F3E5AB"
        cellFormat["bold"] = 1
        #cellFormat["font_color"] = "#FFFFFF"
    return workbook.add_format(cellFormat)

def NumFormat(workbook):
    """Creates Format class for numbers"""
    cellFormat = {"font_name": "Gill Sans MT Ext Condensed Bold",
                  "font_size": 10,
                  "align": "center",
                  "valign": "center"}
    return workbook.add_format(cellFormat)
    
def GetWorksheet(workbook, name):
    for sheet in workbook.worksheets():
        if sheet.name == name:
            return sheet
    return workbook.add_worksheet(name)
    
def OutputMode(workbook, modes, records):
    """Depending on mode, writes alignments in a certain format."""
    iStart = 0
    dStart = 0
    pStart = 0
    max_len = 0
    for aln in records:
        for record in aln:
            max_len = max(max_len, len(record.seq))
    col_width = 1.57 #Set column widths to ~16px
    if 'i' in modes:
        iWorksheet = GetWorksheet(workbook, "Individual")
        iWorksheet.set_column(1, max_len, col_width)
        iStart = Individual(workbook, iWorksheet, record, iStart)
    if 'd' in modes:
        dWorksheet = GetWorksheet(workbook, "DNA Alignments")
        dWorksheet.set_column(1, max_len, col_width)
        for record in records:
            dStart = AlignDNA(workbook, dWorksheet, record, dStart)
    if 'p' in modes:
        pWorksheet = GetWorksheet(workbook, "Protein Alignments")
        pWorksheet.set_column(1, int(ceil(float(max_len)/3)), col_width)
        for record in records:
            pStart = AlignAA(workbook, pWorksheet, record, pStart)
    return (iStart, dStart, pStart)
    
def Individual(workbook, worksheet, seqRecs, start=0):
    """
    Selects first seq as the reference, and aligns it to every other sequence,
    while writing every possible amino acid (even frameshifted) and showing
    matches. Ex.
    Poss AA's M  D  I  G  G  L  E
               W  T  S  E  A  W  R
                G  H  R  R  P  G  D
    GRK1      ATGGACATCGGAGGCCTGGAG...
    Matches   |||:|:|||||||||||||||...
    GRK1_SOAP ATGRAYATCGGAGGCCTGGAG...
                G  H  R  R  P  G  D
               W  T  S  E  A  W  R
    Poss AA's M  D  I  G  G  L  E
    """
    #Get reference's possible proteins
    refProteins = GetPossProteins(seqRecs[0].seq)
    
    #Output alignments
    for aln in range(len(seqRecs)-1):
        #Output ref's amino acids
        worksheet.write("A%d" % (aln*10+1), "Poss. AA's")
        for row, proteinSeq in enumerate(refProteins):
            for col in range(len(proteinSeq)):
                if len(proteinSeq[col]) > 1:
                    worksheet.data_validation(aln*10+row,    #First row
                                              3*col+row+1,   #First col
                                              aln*10+row,    #Last row
                                              3*col+row+1,   #Last col
                                              {"validate": "list",
                                               "source": proteinSeq[col]})
                    multi = True
                else:
                    multi = False
                worksheet.write(aln*10+row,
                                3*col+row+1,
                                proteinSeq[col][0],
                                AAFormat(workbook, proteinSeq[col][0], multi))
        #Output ref sequence, matches, and de novo sequence
        worksheet.write("A%d" % (aln*10+4), seqRecs[0].description)
        for col in range(len(seqRecs[0].seq)):
            worksheet.write(aln*10+3, col+1, seqRecs[0].seq[col],
                            DNAFormat(workbook, seqRecs[0].seq[col]))
        worksheet.write("A%d" % (aln*10+5), "Matches")
        worksheet.write_row("B%d" % (aln*10+5),
                            MatchTwoSeqs(seqRecs[0].seq, seqRecs[aln+1].seq))
        worksheet.write("A%d" % (aln*10+6), seqRecs[aln+1].description)
        for col in range(len(seqRecs[aln+1].seq)):
            worksheet.write(aln*10+5, col+1, seqRecs[aln+1].seq[col],
                            DNAFormat(workbook, seqRecs[aln+1].seq[col]))
        #Output de novo's amino acids
        worksheet.write("A%d" % (aln*10+9), "Poss. AA's")
        for row, proteinSeq in enumerate(GetPossProteins(seqRecs[aln+1].seq)):
            for col in range(len(proteinSeq)):
                if len(proteinSeq[col]) > 1:
                    worksheet.data_validation(aln*10+8-row,
                                              3*col+row+1,
                                              aln*10+8-row,
                                              3*col+row+1,
                                              {"validate": "list",
                                              "source": proteinSeq[col]})
                    multi = True
                else:
                    multi = False
                try:
                    worksheet.write(aln*10+8-row,
                                    3*col+row+1,
                                    refProteins[row][col][0],
                                    AAFormat(workbook,
                                             refProteins[row][col][0],
                                             multi))
                except:
                    worksheet.write(aln*10+8-row,
                                    3*col+row+1,
                                    proteinSeq[col][0],
                                    AAFormat(workbook,
                                             proteinSeq[col][0],
                                             multi))
    return start + 10 * (len(seqRecs) - 1) + 1

def AlignDNA(workbook, worksheet, seqRecs, start=0):
    """Writes DNA alignments MEGA style, returns last writing row"""
    #Output alignments
    worksheet.write(start, 0, "Position")
    max_len = len(max(seqRecs, key=(lambda r: len(r.seq))).seq)
    worksheet.write_row(start, 1 , range(1, max_len+1) ,NumFormat(workbook))
    worksheet.write(start+1, 0, "Matches")
    sequences = [rec.seq for rec in seqRecs]
    worksheet.write_row(start+1, 1, MatchDNASeqs(sequences))
    for row, rec in enumerate(seqRecs):
        worksheet.write(start+row+2, 0, rec.description)
        for col, nt in enumerate(rec.seq):
            worksheet.write(start+row+2, col+1, nt, DNAFormat(workbook, nt))
    return start + len(seqRecs) + 3
            
def AlignAA(workbook, worksheet, seqRecs, start=0):
    """Writes protein alignments MEGA style, returns last writing row"""
    #Output alignments
    worksheet.write(start, 0, "Position")
    max_len = ceil(float(len(max(seqRecs, key=(lambda r: len(r.seq))).seq))/3)
    worksheet.write_row(start, 1, range(1, int(max_len)+1), NumFormat(workbook))
    worksheet.write(start+1, 0, "Matches")
    proteins = [(r.description,GetProteinSeq(r.seq)) for r in seqRecs]
    proteinSeqs = [p[1] for p in proteins]
    worksheet.write_row(start+1, 1, MatchAASeqs(proteinSeqs))
    for row, protein in enumerate(proteins):
        worksheet.write(start+row+2, 0, protein[0])
        for col, AAs in enumerate(protein[1]):
            if len(AAs) > 1:
                worksheet.data_validation(start+row+2,
                                          col+1,
                                          start+row+2,
                                          col+1,
                                          {"validate": "list", "source": AAs})
                match = True
            else:
                match = False
            try:
                if proteins[0][1][col][0] in AAs: #See if ref's AA is possible
                    worksheet.write(start+row+2,
                                    col+1,
                                    proteins[0][1][col][0],
                                    AAFormat(workbook, AAs[0], match))
                else:
                    worksheet.write(start+row+2,
                                    col+1,
                                    AAs[0],
                                    AAFormat(workbook, AAs[0], match))
            except IndexError:
                worksheet.write(start+row+2,
                                col+1,
                                AAs[0],
                                AAFormat(workbook, AAs[0], match))

    return start + len(seqRecs) + 3