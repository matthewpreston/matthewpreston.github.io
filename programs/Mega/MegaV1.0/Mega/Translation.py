from Bio.Data import CodonTable, IUPACData

def GetProteinSeq(seq, offset=0):
    """Translates DNA directly into amino acids with offset"""
    result = []
    for i in range(offset, len(seq), 3):
        if i+3 <= len(seq):
            result.append(PossAA(seq[i:i+3]))
        else:#Codon is not 3 nts long
            result.append(['?'])
    return result

def GetPossProteins(seq):
    """Returns a list of 3 protein sequences which are offset by 0, 1, or 2"""
    return [GetProteinSeq(seq,0),GetProteinSeq(seq,1),GetProteinSeq(seq,2)]

def MatchTwoSeqs(seq1, seq2):
    """
    Returns a list of how the two sequences match
    I.E.
        '|'        Matches exactly
        ':'        Possible match (Y may indicate being a C)
        '.'        Possible match but using N instead
        ' '        No match
    """
    result = []
    for a, b in zip(seq1, seq2):
        if a == b:#
            result.append('|')
        elif a == 'N' or b == 'N':
            result.append('.')
        else:
            ambigDNA = IUPACData.ambiguous_dna_values
            possA = set(ambigDNA[a])
            possB = set(ambigDNA[b])
            if possA.isdisjoint(possB):#No shared elements
                result.append(' ')
            else:#At least 1 shared element
                result.append(':')
    return result

def MatchDNASeqs(seqs):
    """
    Returns a list of how all the sequences are related
    I.E.
        '*'            Matches exactly throughout
        ':'            Possible match (ambiguity)
        ' '            No match
        '-'            Dashes in alignment
        '?'            Some error occured
    """
    result = []
    for tup in zip(*seqs):
        test = set(tup)
        if '-' in test:#Dashes in alignment
            result.append('-')
        elif len(test) == 1:#All the same letter
            result.append('*')
        elif len(test & set(['A','C','G','T'])) > 1:#At least two different
            result.append(' ')
        elif 'A' in test:
            if len(test & set(['K','Y','S','B'])) > 0:#Contains a non-A ambig nt
                result.append(' ')
            else:
                result.append(':')
        elif 'C' in test:
            if len(test & set(['K','R','W','D'])) > 0:#Contains a non-C ambig nt
                result.append(' ')
            else:
                result.append(':')
        elif 'G' in test:
            if len(test & set(['M','Y','W','H'])) > 0:#Contains a non-G ambig nt
                result.append(' ')
            else:
                result.append(':')
        elif 'T' in test:
            if len(test & set(['M','R','S','V'])) > 0:#Contains a non-T ambig nt
                result.append(' ')
            else:
                result.append(':')
        else:#Something weird happened
            result.append('?')
    return result

def MatchAASeqs(seqs):
    """
    Returns a list of how all the sequences are related
    I.E.
        '*'            Matches exactly throughout
        ':'            Possible match (ambiguity)
        ' '            No match
        '-'            Matching dashes or question marks
    """
    result = []
    for tup in zip(*seqs):
        test = set(aa for elem in tup for aa in elem)#Unpack
        if len(test) == 1:#All the same letter
            if len(test & set(['-','?'])) > 0:#Matching dashes/question marks
                result.append('-')
            else:                 #Matching amino acids
                result.append('*')
        else:
            test = set(aa for aa in tup[0])
            for t in tup[1:]:
                test &= set(aa for aa in t)
            if len(test) == 1:#One related amino acid over all sequences
                result.append(':')
            else:
                result.append(' ')
    return result
    
def PossAA(codon, dictionary=False):
    """Returns a list of possible amino acids encoded by ambiguous codon"""
    codonTable = CodonTable.ambiguous_dna_by_id[1].forward_table
    ambigDNA = IUPACData.ambiguous_dna_values
    c1, c2, c3 = codon
    try:
        x1 = ambigDNA[c1]
        x2 = ambigDNA[c2]
        x3 = ambigDNA[c3]
    except KeyError:
        if c1 == '-' and c2 == '-' and c3 == '-':
            if dictionary:
                return {'-': c1+c2+c3}
            else:
                return ['-']
        else:
            if dictionary:
                return {'?': c1+c2+c3}
            else:
                return ['?']
    possible = {}
    for y1 in x1:
        for y2 in x2:
            for y3 in x3:
                try:
                    possible[codonTable[y1+y2+y3]] = y1+y2+y3
                except KeyError:
                    possible['*'] = y1+y2+y3
    if dictionary:
        return possible
    else:
        results = list(possible)
        results.sort()
        return results