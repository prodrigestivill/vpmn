%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%----------------------------------------------------------------------------------------%%%
%%%                          EPSC TFC/PFC document style                                   %%%
%%%----------------------------------------------------------------------------------------%%%
%%% Author: Xavier Prats i Menéndez                                                        %%%
%%% e-mail: xavier.prats-menendez@upc.edu                                                  %%%
%%%----------------------------------------------------------------------------------------%%%
%%% Created for LaTeX 2e                                                                   %%%
%%%----------------------------------------------------------------------------------------%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%----------------------------------------------------------------------------------------%%%
%%%                                  IDENTIFICATION                                        %%%
%%%----------------------------------------------------------------------------------------%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{epsc_tfc_pfc}
              [2006/03/09 v1.4 EPSC TFC/PFC document style]

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%----------------------------------------------------------------------------------------%%%
%%%                                 INITIAL CODE                                           %%%
%%%----------------------------------------------------------------------------------------%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\RequirePackage{ifthen}

\newboolean{twoside}
\newboolean{mparswitch}
\newboolean{@draft}
\newboolean{@final}

\newboolean{ONEauthor}
\newboolean{Nauthors}
\newboolean{TFC}
\newboolean{PFC}
\newboolean{supervisor}
\newboolean{logo}

\setboolean{@draft}{false}
\setboolean{@final}{false}

\setboolean{ONEauthor}{false}
\setboolean{Nauthors}{false}
\setboolean{TFC}{false}
\setboolean{PFC}{false}
\setboolean{supervisor}{false}
\setboolean{logo}{false}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%----------------------------------------------------------------------------------------%%%
%%%                        OPTIONS DECLARATION and EXECUTION                               %%%
%%%----------------------------------------------------------------------------------------%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\DeclareOption{oneside}{\setboolean{twoside}{false} \setboolean{mparswitch}{false}}
\DeclareOption{twoside}{\setboolean{twoside}{true} \setboolean{mparswitch}{true}}

\DeclareOption{draft}{\AtEndOfClass{\setlength\overfullrule{10pt}\setboolean{@draft}{true}}}
\DeclareOption{final}{\AtEndOfClass{\setlength\overfullrule{0pt} \setboolean{@final}{true}}}

\DeclareOption*{\PassOptionsToClass{\CurrentOption}{book}}
%\DeclareOption*{code}  -->  code execution for a unknown option
%\PassOptionsToPackage{option}{pack}
%\CurrentOption

%% EXECUTION OF OPTIONS
% default options if not specified 
\ExecuteOptions{twoside,final}  

\ProcessOptions

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%----------------------------------------------------------------------------------------%%%
%%%                                 PACKAGE LOADING                                        %%%
%%%----------------------------------------------------------------------------------------%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\LoadClass{book}

%%% FancyHeadings and  Calc packages are required
\RequirePackage{fancyhdr}
\RequirePackage{calc}
\RequirePackage{graphicx}
\RequirePackage{tocloft}  %% controls typografic design of the TOC, LOF etc...
\RequirePackage{pslatex}  %% Arial
\RequirePackage{helvet}

\renewcommand{\familydefault}{\sfdefault}  %% sans-serif is default


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\renewcommand{\and}{\\ \hspace*{\Lautors}}
\input{epsc/dades}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%----------------------------------------------------------------------------------------%%%
%%%                              DOCUMENT CLASS DEFINITIONS                                %%%
%%%----------------------------------------------------------------------------------------%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%\ifthenelse{test}{thencode}{elsecode}
%%\equal{string1}{string2}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% PAGE LAYOUT                                                                              %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% NOTE!!: All margin dimensions are measured from a point one inch from top and side of page.  

% HORIZONTAL SPACING
%%%%%%%%%%%%%%%%%%%%

\ifthenelse{\boolean{twoside}}{% Values for two-sided printing:
  \setlength{\oddsidemargin}{0.46cm}      %   Left margin on odd-numbered pages.
  \setlength{\evensidemargin}{0.46cm}     %   Left margin on even-numbered pages.
  \setlength{\marginparwidth}{0cm}        %   Width of marginal notes.
  \setlength{\marginparsep}{0pt}            % Horizontal space between outer margin and marginal note
}
{% Values for one-sided printing:
  \setlength{\oddsidemargin}{0.46cm}      %   Left margin on odd-numbered pages.
  \setlength{\evensidemargin}{0.46}       %   Left margin on even-numbered pages.
  \setlength{\marginparwidth}{0cm}        %   Width of marginal notes.
  \setlength{\marginparsep}{0pt}            % Horizontal space between outer margin and marginal note
}
%\setlength{\hoffset}{-1cm}

% VERTICAL SPACING
%%%%%%%%%%%%%%%%%%
% Top of page:
\setlength{\headheight}{1\baselineskip}  %    Height of box containing running head.
\setlength{\headsep}{8mm}                %    Space between running head and text.
\setlength{\topmargin}{-0.04cm - \headheight - \headsep}          
                                         %    Nominal distance from top of page to top of box containing running head.
% Bottom of page:
\setlength{\footskip}{25mm}       %    Distance from baseline of box containing foot to baseline of last line of text.

% DIMENSION OF TEXT
%%%%%%%%%%%%%%%%%%%

\setlength{\textheight}{24.6cm}    % \textheight is the height of text 
                                   %(including footnotes and figures, excluding running head and foot).
\setlength{\textwidth}{15cm}       % Width of text line.

% A \raggedbottom command causes 'ragged bottom' pages: pages set to
% natural height instead of being stretched to exactly \textheight.
\raggedbottom

% FOOTNOTES
%%%%%%%%%%%
\setlength{\footnotesep}{8.4pt}    % Height of strut placed at the beginning of every footnote = 
                                   % height of normal \footnotesize strut, so no extra space between footnotes.
%%\skip\footins 10.8pt plus 4pt minus 2pt  % Space between last line of text and 
                                         % top of first footnote.

% FLOATS: (a float is something like a figure or table)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% FOR FLOATS ON A TEXT PAGE
%    ONE-COLUMN MODE OR SINGLE-COLUMN FLOATS IN TWO-COLUMN MODE:
\setlength{\floatsep}{14pt plus 2pt minus 4pt}     % Space between adjacent floats moved  to top or bottom of text page.
\setlength{\textfloatsep}{20pt plus 2pt minus 4pt} % Space between main text and floats at top or bottom of page.
\setlength{\intextsep}{14pt plus 4pt minus 4pt}    % Space between in-text figures and text
%    TWO-COLUMN FLOATS IN TWO-COLUMN MODE:
\setlength{\dblfloatsep}{14pt plus 2pt minus 4pt}  % Same as \floatsep for double-column figures in two-column mode.
\setlength{\dbltextfloatsep}{20pt plus 2pt minus 4pt}% \textfloatsep for double-column floats.


%  FOR FLOATS ON A SEPARATE FLOAT PAGE OR COLUMN
%    ONE-COLUMN MODE OR SINGLE-COLUMN FLOATS IN TWO-COLUMN MODE:
\setlength{\@fptop}{0pt plus 1fil}      % Stretch at top of float page/column. (Must be 0pt plus ...)          
\setlength{\@fpsep}{10pt plus 2fil}     % Space between floats on float page/column.       
\setlength{\@fpbot}{0pt plus 1fil}      % Stretch at bottom of float page/column. (Must be 0pt plus ... )    
%   DOUBLE-COLUMN FLOATS IN TWO-COLUMN MODE.
\setlength{\@dblfptop}{0pt plus 1fil}   % Stretch at top of float page. (Must be 0pt plus ...)
\setlength{\@dblfpsep}{10pt plus 2fil}  % Space between floats on float page.
\setlength{\@dblfpbot}{0pt plus 1fil}   % Stretch at bottom of float page. (Must be 0pt plus ... )     

% MARGINAL NOTES
%%%%%%%%%%%%%%%%
\setlength{\marginparpush}{7pt}       % Minimum vertical separation between two marginal notes.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% NEW ENVIRONMENT DEFINITIONS                                                              %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\newlength{\Lautors}

\newlength{\logoEPSCheight}   %% no tocar. nomes descomentar
\setlength{\logoEPSCheight}{1.5cm}
\newsavebox{\logoEPSC}
\sbox{\logoEPSC}{\includegraphics[totalheight=\logoEPSCheight]{epsc/logo_epsc_complert.pdf}}

%% PORTADA ENVIRONMENT
%%%%%%%%%%%%%%%%%%%%%%
\newcommand{\portada}{
\settowidth{\Lautors}{{\normalsize\bf\sffamily AUTORS: }}
\begin{tabular}{p{\textwidth}}
\ifthenelse{\boolean{logo}}{\usebox{\logoEPSC}  \hfill \usebox{\logo}   \\[8cm]}{\usebox{\logoEPSC} \\[8cm]}
\ifthenelse{\boolean{TFC}}{\begin{center}\Huge\sffamily\bfseries  TREBALL DE FI DE CARRERA \end{center}  \\[5cm]}{}
\ifthenelse{\boolean{PFC}}{\begin{center}\Huge\sffamily\bfseries  PROJECTE DE FI DE CARRERA \end{center}  \\[5cm]}{}
\fbox{\parbox{\textwidth}{
        \vspace{2ex}
        \ifthenelse{\boolean{TFC}}{{\normalsize\bf\sffamily  T\'ITOL DEL TFC : \titledoc}  \\[2ex]}{}
        \ifthenelse{\boolean{PFC}}{{\normalsize\bf\sffamily  T\'ITOL DEL PFC : \titledoc}  \\[2ex]}{}
        {\normalsize\bf\sffamily  TITULACI\'O: \titulacio    }  \\[2ex]
        \ifthenelse{\boolean{ONEauthor}}{{\normalsize\bf\sffamily AUTOR: \authordoc  } \\[2ex]}{}
        \ifthenelse{\boolean{Nauthors}}{{\normalsize\bf\sffamily AUTORS: \authordoc  } \\[2ex]}{}
        {\normalsize\bf\sffamily  DIRECTOR:    \directordoc  }  \\[2ex]
        \ifthenelse{\boolean{supervisor}}{{\normalsize\bf\sffamily SUPERVISOR: \supervisordoc  } \\[2ex]}{}
        {\normalsize\bf\sffamily  DATA: \today               }  \\[2ex]
      }} \\

\end{tabular}
\cleardoublepage
}



%% RESUM ENVIRONMENT
%%%%%%%%%%%%%%%%%%%%%%
\newenvironment{resum}[1]
{
\newlength{\Hresum}
\setlength{\Hresum}{#1}
\settowidth{\Lautors}{{\bf Autors: }}
  \begin{center}
    \fbox{
      \parbox{\textwidth}{
        \vspace{2ex}
        {\bf T\'itol :}   \titledoc    \\[1ex]
        \ifthenelse{\boolean{ONEauthor}}{{\bf Autor:} \authordoc   \\[1ex]}{}
        \ifthenelse{\boolean{Nauthors}}{{\bf Autors:} \authordoc   \\[1ex]}{}
        {\bf Director:}   \directordoc \\[1ex]
        \ifthenelse{\boolean{supervisor}}{{\bf Supervisor:} \supervisordoc   \\[1ex]}{}
        {\bf Data:}       \today       \\[1ex]
      }}
    \vspace{1em}
    \begin{flushleft}
      Resum
    \end{flushleft}
    \vspace{-0.9em}
    \begin{tabular}{|p{\textwidth}|}
      \hline
      \vspace{0.5em}
    }
    {
         \vspace*{\Hresum} \\
         \hline
       \end{tabular}
       \end{center}
     \cleardoublepage
    }

%% OVERVIEW ENVIRONMENT
%%%%%%%%%%%%%%%%%%%%%%%%
\newenvironment{overview}[1]
{
\newlength{\Hoverview}
\setlength{\Hoverview}{#1}
\settowidth{\Lautors}{{\bf Authors: }}
  \begin{center}
    \fbox{
      \parbox{\textwidth}{
        \vspace{2ex}
        {\bf Title :}     \titledocE   \\[1ex]
        \ifthenelse{\boolean{ONEauthor}}{{\bf Author:} \authordoc   \\[1ex]}{}
        \ifthenelse{\boolean{Nauthors}}{{\bf Authors:} \authordoc   \\[1ex]}{}
        {\bf Director:}   \directordoc \\[1ex]
        \ifthenelse{\boolean{supervisor}}{{\bf Supervisor:} \supervisordoc   \\[1ex]}{}
        {\bf Date:}       \today       \\[1ex]
      }}
    \vspace{1em}
    \begin{flushleft}
      Overview
    \end{flushleft}
    \vspace{-0.9em}
    \begin{tabular}{|p{\textwidth}|}
      \hline
      \vspace{0.5em}
      
    }
    {
         \vspace*{\Hoverview} \\
         \hline
       \end{tabular}
       \end{center}
      \cleardoublepage
    }


%% DEDICATORIA ENVIRONMENT
%%%%%%%%%%%%%%%%%%%%%%%%%%
\newenvironment{dedicatoria}
{
  \vspace*{5cm}
  \begin{flushright}
}
{
  \end{flushright}
    \cleardoublepage
}

\newenvironment{intro}[1]
{
\chapter*{#1}
\setcounter{page}{1}
}
{
}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% NEW COMMANDS or MACRO DEFINITIONS                                                        %%   
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Xevi's macros for brackets...
\newcommand{\lp}{\left(}
\newcommand{\lc}{\left[}
\newcommand{\lcl}{\left\{}
\newcommand{\rp}{\right)}
\newcommand{\rc}{\right]}
\newcommand{\rcl}{\right\}}


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% HEADERS AND FOOTERS DESIGN                                                               %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\renewcommand\headrulewidth{0.5pt}
\renewcommand\footrulewidth{0pt}

\pagestyle{fancy}
%\addtolength{\headwidth}{\marginparwidth+\marginparsep}   % Span headwidth to marginal notes

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% REDEFINITION OF PAGE MARKERS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\renewcommand{\sectionmark}[1]{\markright{#1}}
\renewcommand{\chaptermark}[1]{\markboth{\footnotesize #1}{#1}}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% HEADERS AND FOOTERS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\lhead[\fancyplain{}{\footnotesize \thepage}]{\fancyplain{\leftmark}{\leftmark}}
\chead{}
\rhead[\fancyplain{}{\footnotesize \titledoc}]{\fancyplain{}{\footnotesize \thepage}}
\lfoot{}
\cfoot{}
\rfoot{}


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% TABLE OF CONTENTS DEFINITION                                                             %%   
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\tocloftpagestyle{empty}
    
%% TOC title configuration
%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\setlength{\cftbeforetoctitleskip}{0ex}
\setlength{\cftaftertoctitleskip}{0ex}
\renewcommand{\cfttoctitlefont}{\LARGE\sffamily\bfseries\MakeUppercase}
\renewcommand{\cftaftertoctitle}{\\}

%% LOF title configuration
%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\setlength{\cftbeforeloftitleskip}{0pt}
\setlength{\cftafterloftitleskip}{0ex}
\renewcommand{\cftloftitlefont}{\LARGE\sffamily\bfseries\MakeUppercase}
\renewcommand{\cftafterloftitle}{\\}

%% LOT title configuration
%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\setlength{\cftbeforelottitleskip}{0pt}
\setlength{\cftafterlottitleskip}{0ex}
\renewcommand{\cftlottitlefont}{\LARGE\sffamily\bfseries\MakeUppercase}
\renewcommand{\cftafterlottitle}{\\}

% TOC chapter configuration
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Chapters
\setlength{\cftbeforechapskip}{2em}   
\setlength{\cftchapindent}{0em}
\setlength{\cftchapnumwidth}{7em}
\renewcommand{\cftchapleader}{\bfseries\cftsecleader}
\renewcommand{\cftchapfont}{\large\sffamily\bfseries}
\renewcommand{\cftchappagefont}{\large\sffamily\bfseries} 
\renewcommand{\cftchappresnum}{\MakeUppercase \@chapapp{} \space}%
\renewcommand{\cftchapaftersnum}{. \space}%
%\renewcommand{\cftchapaftersnumb}{}%

%% Sections
\setlength{\cftbeforesecskip}{1em}   
\setlength{\cftsecindent}{0em}
\setlength{\cftsecnumwidth}{2em}
\renewcommand{\cftsecfont}{\normalsize\sffamily\bfseries}
\renewcommand{\cftsecpagefont}{\normalsize\sffamily\bfseries} 
\renewcommand{\cftsecpresnum}{}%
\renewcommand{\cftsecaftersnum}{}%
%\renewcommand{\cftsecaftersnumb}{}%

%% Subsections
\setlength{\cftbeforesubsecskip}{0.2em}   
\setlength{\cftsubsecindent}{3em}
\setlength{\cftsubsecnumwidth}{3em}
\renewcommand{\cftsubsecfont}{\normalsize\sffamily}
\renewcommand{\cftsubsecpagefont}{\normalsize\sffamily} 
\renewcommand{\cftsubsecpresnum}{}%
\renewcommand{\cftsubsecaftersnum}{}%
%\renewcommand{\cftsecaftersnumb}{}%

% Table of figures and table of tables
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\renewcommand{\l@figure}{\@dottedtocline{1}{0em}{2em}}
\renewcommand{\l@table}{\l@figure}

%%%% specifies the highest level appearing in the table of contents
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\setcounter{tocdepth}{2}
%\setcounter{minitocdepth}{4}         % idem for mini table of contents

%%%% configures minitoc font properties
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%\setlength{\mtcindent}{24pt}
%\renewcommand{\mtifont}{\huge\sffamily\bfseries}
%\renewcommand{\mtcfont}{\normalsize\rm}
%\nomtcrule   %%% supress rule in minitoc

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% PARAGRAPHING                                                                             %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 
\setlength{\parskip}{2ex}          % Extra vertical space between paragraphs (added to \baselineskip)
\setlength{\parindent}{0em}        % Width of paragraph indentation.

\setlength{\topsep}{0.5ex}         % Extra vertical space, in addition to 
                                   % \parskip, added above and below list and
                                   % paragraphing environments.
\setlength{\partopsep}{0.5ex}      % Extra vertical space, in addition to 
                                   % \parskip and \topsep, added when user
                                   % leaves blank line before environment.

\setlength{\itemsep}{1pt}          % Extra vertical space, in addition to 
                                   % \parskip, added between list items.

% The following page-breaking penalties are defined

%\@lowpenalty   51      % Produced by \nopagebreak[1] or \nolinebreak[1]
%\@medpenalty  151      % Produced by \nopagebreak[2] or \nolinebreak[2]
%\@highpenalty 301      % Produced by \nopagebreak[3] or \nolinebreak[3]

%\@beginparpenalty -\@lowpenalty    % Before a list or paragraph environment.
%\@endparpenalty   -\@lowpenalty    % After a list or paragraph environment.
%\@itempenalty     -\@lowpenalty    % Between list items.

% \clubpenalty         % 'Club line'  at bottom of page.
% \widowpenalty        % 'Widow line' at top of page.
% \displaywidowpenalty % Math display widow line.
% \predisplaypenalty   % Breaking before a math display.
% \postdisplaypenalty  % Breaking after a math display.
% \interlinepenalty    % Breaking at a line within a paragraph.
% \brokenpenalty       % Breaking after a hyphenated line.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% HEADINGS OF CHAPTERS AND SECTIONS FORMATTING                                             %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% COUNTER DEFINITIONS
%%%%%%%%%%%%%%%%%%%%%%
%% If you want to redefine an already existing counter use \@addtoreset  command
%
%%%% specifies the highest level with numbered headings
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\setcounter{secnumdepth}{4}

%% CHAPTER HEADINGS
%%%%%%%%%%%%%%%%%%%
% DEFINITION OF THE NUMBERING OF THE HEADERS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\renewcommand{\thechapter}{\arabic{chapter}}
\renewcommand{\thesection}{\thechapter .\arabic{section}.}
\renewcommand{\thesubsection}{\thesection \arabic{subsection}.}
\renewcommand{\thesubsubsection}{\thesubsection \arabic{subsubsection}.}
\renewcommand{\theparagraph}{\alph{paragraph}}
\renewcommand{\thesubparagraph}{-}

% \@makechapterhead Makes the heading for the \chapter command.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\renewcommand{\@makechapterhead}[1]{%
  \noindent                             % No indentation in this paragraph
  \thispagestyle{fancy}
  \begin{center}                    % Prints chapter name etc...
    \LARGE\sffamily\bfseries\MakeUppercase 
    \@chapapp{} \thechapter . \MakeUppercase{#1} \par\nobreak  %% \@chapapp{} prints ``CHAPTER'' according to selected language
  \end{center}                     
  \vspace{3ex}
}

% \@makechapterhead Makes the heading for the \chapter* command.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\renewcommand{\@makeschapterhead}[1]{%
  \noindent                                                     % No indentation in this paragraph
  \thispagestyle{fancy}
  \addcontentsline{toc}{chapter}{#1}
  \begin{center}
    \LARGE\sffamily\bfseries \MakeUppercase{#1} \par\nobreak
  \end{center}
  \vspace{3ex}
}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% SECTIONS HEADINGS
%%%%%%%%%%%%%%%%%%%%

% \@startsection {NAME}{LEVEL}{INDENT}{BEFORESKIP}{AFTERSKIP}{STYLE} 
%            optional * [ALTHEADING]{HEADING}
%    Generic command to start a section.  
%    NAME       : e.g., 'subsection'
%    LEVEL      : a number, denoting depth of section -- e.g., chapter=1,
%                 section = 2, etc.  A section number will be printed if
%                 and only if LEVEL < or = the value of the secnumdepth
%                 counter.
%    INDENT     : Indentation of heading from left margin
%    BEFORESKIP : Absolute value = skip to leave above the heading.  
%                 If negative, then paragraph indent of text following 
%                 heading is suppressed.
%    AFTERSKIP  : if positive, then skip to leave below heading,
%                       else - skip to leave to right of run-in heading.
%    STYLE      : commands to set heading style 
%  If '*' missing, then increments the counter.  If it is present, then
%  there should be no [ALTHEADING] argument.  A sectioning command
%  is normally defined to \@startsection + its first six arguments.

%% REDEFINITION OF ALREADY EXISTING SECTIONS (book.cls)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\renewcommand{\section}{%
  \@startsection {section}%                          %% NAME
                 {1}{0pt}%                           %% LEVEL and INDENT
                 {-3.5ex plus -1ex minus -.2ex}%    %% BEFORESKIP
                 {2.3ex plus .2ex}%                  %% AFTERSKIP
                 {\sffamily\Large\bfseries}%         %% STYLE
}

\renewcommand{\subsection}{%
  \@startsection{subsection}%                        %% NAME
                {2}{0pt}%                            %% LEVEL and INDENT
                {-3.25ex plus -1ex minus -.2ex}%     %% BEFORESKIP
                {1.5ex plus .2ex}%                   %% AFTERSKIP
                {\sffamily\large\bfseries}%          %% STYLE
}

\renewcommand{\subsubsection}{%
  \@startsection{subsubsection}%                     %% NAME
                {3}{0pt}%                            %% LEVEL and INDENT
                {-3.25ex plus -1ex minus -.2ex}%     %% BEFORESKIP
                {1.0ex plus .2ex}%                   %% AFTERSKIP
                {\sffamily\normalsize\itshape}%      %% STYLE
}

\renewcommand{\paragraph}{%
  \@startsection{paragraph}%                             %% NAME
                {4}{2ex}%                                %% LEVEL and INDENT
                {3.25ex plus 1ex minus .2ex}%            %% BEFORESKIP
                {1ex}%                                   %% AFTERSKIP
                {\normalsize\bfseries} %        %% STYLE
}

\renewcommand{\subparagraph}{%
  \@startsection{subparagraph}%                      %% NAME
                {5}{3ex}%                            %% LEVEL and INDENT
                {3.25ex plus 1ex minus .2ex}%        %% BEFORESKIP
                {1ex}%                              %% AFTERSKIP
                {\normalsize\bfseries}%                       %% STYLE
}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% APPENDIX CONFIGURATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\DeclareRobustCommand{\changetoapp}{%
  \setlength{\cftbeforechapskip}{2em}  
  \setlength{\cftchapindent}{0em}
  \setlength{\cftchapnumwidth}{7em}
  \renewcommand{\cftchapleader}{\bfseries\cftsecleader}
  \renewcommand{\cftchapfont}{\large\sffamily\bfseries}
  \renewcommand*{\cftchappresnum}{\MakeUppercase \appendixname \space}%
  \renewcommand{\cftchapaftersnum}{. \space}
  \renewcommand{\cftchapaftersnumb}{}%
}

\let\oldappendix\appendix

\renewcommand*{\appendix}{
  \oldappendix 
  \addtocontents{toc}{\changetoapp}
 % \addtocontents{toc}{\protect\clearpage\par}
%  \addtocontents{toc}{{\Large\sffamily\bfseries Annexos}\par}
%  \addtocontents{toc}{\protect\apprule \par}
  
  \addtocontents{lof}{\changetoapp}
%  \addtocontents{lof}{\protect\clearpage\par}
%  \addtocontents{lof}{{\Large\sffamily\bfseries Annexos}\par}
 % \addtocontents{lof}{\protect\apprule \par}

  \addtocontents{lot}{\changetoapp} 
%  \addtocontents{lot}{\protect\clearpage\par}
%  \addtocontents{lot}{{\Large\sffamily\bfseries Annexos}\par}
%  \addtocontents{lot}{\protect\apprule \par}


  \cleardoublepage
  
}

\newcommand{\senseportada}{
  \vspace*{\stretch{1}}            
  \phantomsection
  \addcontentsline{toc}{part}{AP\`ENDIXS}
  \begin{center}                    % Prints chapter name etc...
    \Huge\sffamily\bfseries AP\`ENDIXS  \par
  \end{center}                     
  \vspace*{\stretch{2}}                
  \thispagestyle{empty}                 % Page style
  \newpage
  \thispagestyle{empty}                 % Page style
  \newpage
}

\newcommand{\ambportada}{
\settowidth{\Lautors}{{\normalsize\bf\sffamily AUTORS: }}
\begin{tabular}{p{\textwidth}}
\ifthenelse{\boolean{logo}}{\usebox{\logoEPSC}  \hfill \usebox{\logo}   \\[8cm]}{\usebox{\logoEPSC} \\[8cm]}
\ifthenelse{\boolean{TFC}}{\begin{center}\Huge\sffamily\bfseries  AP\`ENDIXS \end{center}  \\[5cm]}{}
\ifthenelse{\boolean{PFC}}{\begin{center}\Huge\sffamily\bfseries  AP\`ENDIXS \end{center}  \\[5cm]}{}
\fbox{\parbox{\textwidth}{
        \vspace{2ex}
        \ifthenelse{\boolean{TFC}}{{\normalsize\bf\sffamily  T\'ITOL DEL TFC : \titledoc}  \\[2ex]}{}
        \ifthenelse{\boolean{PFC}}{{\normalsize\bf\sffamily  T\'ITOL DEL PFC : \titledoc}  \\[2ex]}{}
        {\normalsize\bf\sffamily  TITULACI\'O: \titulacio    }  \\[2ex]
        \ifthenelse{\boolean{ONEauthor}}{{\normalsize\bf\sffamily AUTOR: \authordoc  } \\[2ex]}{}
        \ifthenelse{\boolean{Nauthors}}{{\normalsize\bf\sffamily AUTORS: \authordoc  } \\[2ex]}{}
        {\normalsize\bf\sffamily  DIRECTOR:    \directordoc  }  \\[2ex]
        \ifthenelse{\boolean{supervisor}}{{\normalsize\bf\sffamily SUPERVISOR: \supervisordoc  } \\[2ex]}{}
        {\normalsize\bf\sffamily  DATA: \today               }  \\[2ex]
      }} \\

\end{tabular}
\setcounter{page}{-1}
}


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%----------------------------------------------------------------------------------------%%%
%%%----------------------------   END OF CLASS FILE  --------------------------------------%%%
%%%----------------------------------------------------------------------------------------%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
