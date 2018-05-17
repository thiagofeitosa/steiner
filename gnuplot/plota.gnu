set title "Árvore de Steiner"

#Seta tamanho da janela automaticamente de acordo com os dados
set autoscale

plot 'edgefixo.dat' u 1:2 with lines lc rgb "black" lw 2 notitle,\
'ptfixos.dat' u 1:2:(0.5) with circles fill solid lc rgb "black" notitle,\
'ptfixos.dat' using 1:2:($0) with labels tc rgb "white" offset (0,0) font 'Arial Bold' notitle,\
'edgesteiner.dat' u 1:2 with lines lc rgb "red" lw 2 notitle,\
'ptsteiner.dat' u 1:2:(0.5) with circles fill solid lc rgb "red" notitle,\
'ptsteiner.dat' using 1:2:($0) with labels tc rgb "white" offset (0,0) font 'Arial Bold' notitle

pause -1
