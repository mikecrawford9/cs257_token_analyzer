create table Concepts ( ID number not null, Tokens varchar2(100) primary key , TokenCount number not null, Frequency number not null, DocFrequency number not null, Perm number not null);

create table Config ( ThreadCount number not null, ParagraphMax number not null, TupleCount number not null, TermFreqFrom number(18,0) not null, TermFreqTo number(18,0) not null, DocFreqFrom number(18,0) not null, DocFreqTo number(18,0), RunningDocCount number not null);

create table URLPages ( ID number not null, URL varchar2(900) primary key, WordCount number not null, LastScan timestamp, Cache char(1000));

create table MapConceptToURL ( ConceptID number primary key, URLID number not null);

