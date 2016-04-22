NAME=dokumentace
CITACIE=literatura.bib

all: img
	vlna -v KkSsVvZzOoUuAIai $(NAME).tex
	vlna -v KkSsVvZzOoUuAIai $(CITACIE) 

	latex $(NAME).tex

	bibtex $(NAME).aux

	latex $(NAME).tex
	latex $(NAME).tex

	dvips -t a4 $(NAME).dvi
	ps2pdf $(NAME).ps

.PHONY: img clean img_clean

clean: img_clean
	rm -f $(NAME).pdf $(NAME).ps $(NAME).dvi $(NAME).log $(NAME).aux missfont.log *.bbl *.blg *.toc

img:
	dia -t eps img/src/*.dia -O img

img_clean:
	rm -f img/scanner_fsm_*.eps img/instruction_list.eps img/call_stack.eps

pack: 
	zip xpokor32.zip dokumentace.pdf

