NAME=prezentace

all:
#	vlna -v -f $(NAME).tex $(NAME).tex
	latex $(NAME).tex
	latex $(NAME).tex
	dvips -t a4 $(NAME).dvi
	ps2pdf $(NAME).ps


clean:
	rm -f $(NAME).pdf $(NAME).ps $(NAME).dvi $(NAME).log $(NAME).aux missfont.log $(NAME).out

