# cd ~/calismalar/pisido
lupdate ../pisido.pro -ts pisido_de_DE.ts -no-obsolete
linguist pisido_de_DE.ts
lrelease -compress pisido_de_DE.ts -qm pisido_de_DE.qm 
