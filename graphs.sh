#!/bin/zsh

data=/home/eduardo/Decisao/data
graficos=/home/eduardo/Decisao/tese/graficos
_GRAFICO=$(dirname $0)/graficos.py

function NOP() { }
function XGRAFICO { }

function GRAFICO {
   echo -n '.'
   $_GRAFICO "$@"
}

legenda=(
   '$x, q^A$'
   '$x, q^C$'
   '$x, q^B$'
   '$xy, q^A$'
   '$xy, q^C$'
   '$\rho\sigma, q^A$'
   '$\rho\sigma, q^C$'
   '$\rho\sigma, {q^C}^\prime$')

typeset -A leg
leg[xxs]=$legenda[1]
leg[xxi]=$legenda[2]
leg[xxb]=$legenda[3]
leg[xys]=$legenda[4]
leg[xyi]=$legenda[5]
leg[wss]=$legenda[6]
leg[wsi]=$legenda[7]
leg[wsj]=$legenda[8]

GRAFICO simcumer \
   $data/m1b/{wss,wsi,wsj}.avg \; ${legenda[6]} ${legenda[7]} ${legenda[8]} \; \
   100 $graficos/m1b/sim1r_w_100.pdf
GRAFICO simcume \
   $data/m1b/{wss,wsi,wsj}.avg \; ${legenda[6]} ${legenda[7]} ${legenda[8]} \; \
   100 $graficos/m1b/sim1_w_100.pdf

echo; exit
GRAFICO simcumer \
   $data/m0/{wss,wsi,wsj}.avg \; ${legenda[6]} ${legenda[7]} ${legenda[8]} \; \
   100 $graficos/m0/sim1r_w_100.pdf
GRAFICO simcume \
   $data/m0/{wss,wsi,wsj}.avg \; ${legenda[6]} ${legenda[7]} ${legenda[8]} \; \
   100 $graficos/m0/sim1_w_100.pdf
echo; exit

XGRAFICO q 5 $graficos/qcomp_05.pdf
XGRAFICO q 20 $graficos/qcomp_20.pdf

GRAFICO simcumer \
   $data/m0/{xxs,wss}.avg \; ${legenda[1]} ${legenda[6]} \; \
   100 $graficos/sim1r_xw_100.pdf
GRAFICO simcume \
   $data/m0/{xxs,wss}.avg \; ${legenda[1]} ${legenda[6]} \; \
   100 $graficos/sim1_xw_100.pdf

GRAFICO simcume \
   $data/m0/{xxs,xxi,xxb,xys,xyi,wss,wsi}.000 \; \
   \; \
   40 $graficos/sim1_000_40.pdf
GRAFICO simcume \
   $data/m0/{xxs,xxi,xxb,xys,xyi,wss,wsi}.000 \; ${legenda[@]} \; \
   400 $graficos/sim1_000_400.pdf
GRAFICO simcume \
   $data/m0/{xxs,xxi,xxb,xys,xyi,wss,wsi}.000 \; ${legenda[@]} \; \
   1000 $graficos/sim1_000_1000.pdf

GRAFICO simcume \
   $data/m0/{xxs,xxi,xxb,xys,xyi,wss,wsi}.avg \; ${legenda[@]} \; \
   40 $graficos/sim1_avg_40.pdf
GRAFICO simcume \
   $data/m0/{xxs,xxi,xxb,xys,xyi,wss,wsi}.avg \; ${legenda[@]} \; \
   400 $graficos/sim1_avg_400.pdf

GRAFICO simcumer \
   $data/m0/{xxs,xxi,xxb,xys,xyi,wss,wsi}.avg \; ${legenda[@]} \; \
   40 $graficos/sim1r_avg_40.pdf
GRAFICO simcumer \
   $data/m0/{xxs,xxi,xxb,xys,xyi,wss,wsi}.avg \; ${legenda[@]} \; \
   400 $graficos/sim1r_avg_400.pdf

echo
