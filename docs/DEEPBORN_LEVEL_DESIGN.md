# Deepborn - Level Design

## Scopo

Questo documento descrive la progressione dei livelli di **Deepborn**.

La fonte sintetica del game design resta `docs/GAME_DESIGN_SPEC.md`. Questo file entra piu' nel dettaglio su identita', ambiente, nemici, gameplay, mutazioni e obiettivi di ogni livello.

## Struttura Della Run

La run completa e' divisa in due meta' speculari:

1. **Discesa nel dungeon**: la larva scende per divorare, mutare e diventare abbastanza forte.
2. **Risalita in superficie**: la creatura torna nel mondo umano e lo invade.

La numerazione dei dungeon level e' fisica, non classica:

- **Dungeon Level 5** e' il piano piu' vicino alla superficie.
- **Dungeon Level 1** e' il cuore piu' profondo e pericoloso del dungeon.

La progressione completa e':

| Livello | Nome | Tema | Funzione |
| --- | --- | --- | --- |
| Dungeon Level 5 | La Soglia Spezzata | Confine superficie/dungeon | Tutorial, prime mutazioni |
| Dungeon Level 4 | La Cripta Marcia | Morte e decomposizione | Prime orde vere, arcieri, ghoul |
| Dungeon Level 3 | Il Nido Antico | Sciame e predazione | Movimento, sciami, build fisiche |
| Dungeon Level 2 | Le Sale del Culto | Magia, rituale, controllo | Caster, target prioritari, psichico |
| Dungeon Level 1 | Il Cuore Sepolto | Sigillo, origine, mostruosita' antica | Boss dungeon, classe primaria |
| Surface Level 6 | I Campi Esterni | Prima invasione | Power fantasy controllato |
| Surface Level 7 | Il Villaggio | Panico organizzato | Campane, barricate, fuoco |
| Surface Level 8 | Caserma e Mura Interne | Esercito umano | Counter-build, unita' specializzate |
| Surface Level 9 | La Fortezza del Lord | Ultima difesa | Test finale della build |
| Surface Level 10 | Il Campione del Regno | Boss finale | Cavaliere sacro mutato |

## Prologo: La Stanza Della Caduta

Il gioco inizia con la larva aliena che si risveglia in una sala distrutta del dungeon. Sopra di lei c'e' una scala verso la luce. Sotto di lei c'e' una discesa buia.

Il player puo' muoversi subito e vede:

- una scala verso la superficie;
- una scala verso il basso;
- resti di scheletri;
- sangue secco;
- carcasse;
- piccole creature da divorare.

Monologo target:

> La luce sopra di me e' viva. Calda. Armata.  
> Questo corpo non basta.  
> Troppo molle. Troppo piccolo. Troppo affamato.  
> Se salgo ora, saro' schiacciato.  
> Sotto, invece, c'e' carne antica. Magia morta. Ossa che ancora si muovono.  
> Scendero'. Divorero'. Cambiero'.  
> Poi tornero'.

Il player puo' comunque salire subito. Se lo fa, entra in una piccola area di superficie dove viene massacrato da guardie, arcieri e un cavaliere.

Questa scena deve durare poco. Deve insegnare una cosa chiara:

**La superficie e' l'obiettivo finale, non l'inizio.**

Dopo la morte, il player riparte dal Dungeon Level 5.

## Dungeon Level 5: La Soglia Spezzata

### Identita'

Questo e' il primo vero livello giocabile. E' il piano piu' vicino alla superficie: abbandonato, vecchio, parzialmente crollato. Non e' il cuore oscuro del dungeon, ma la sua periferia.

Arrivano ancora rumori dal villaggio: campane lontane, passi sopra la pietra, acqua che filtra dal terreno.

Il tema e':

**Confine tra mondo umano e dungeon.**

La larva non combatte grandi demoni. Sopravvive contro resti, parassiti e piccoli non-morti.

### Ambiente

- corridoi di pietra rotta;
- celle abbandonate;
- ossa sparse;
- torce quasi spente;
- radici che entrano dal soffitto;
- pozze d'acqua;
- piccole aperture con luce dalla superficie;
- porte arrugginite;
- catene appese;
- cadaveri secchi.

Il livello deve sembrare un luogo dimenticato sotto un villaggio medievale, non ancora un inferno.

### Nemici

- **Scheletri rotti**: lenti, fragili, insegnano il combattimento base.
- **Topi mutati**: veloci, poco danno, insegnano a non farsi circondare.
- **Melme piccole**: rallentano il player.
- **Pipistrelli ciechi**: arrivano in piccoli gruppi dall'alto.
- **Cadaveri striscianti**: nemici mezzi morti che si trascinano.
- **Resti animati**: mucchi di ossa che si ricompongono.

Elite:

**Jailer Husk**, il carceriere morto. Era un vecchio guardiano del dungeon. Ora e' un corpo secco con chiavi fuse nelle mani. Protegge la scala verso il Dungeon Level 4.

### Gameplay

Questo livello insegna:

- movimento;
- raccolta biomassa;
- scelta carte;
- orde leggere;
- concetto di fame;
- prima mutazione visibile;
- rischio di essere circondati.

Il player parte con:

- morso debole;
- scatto corto;
- raccolta biomassa automatica a corto raggio;
- HP bassi;
- alta velocita';
- hitbox piccola.

### Prime Mutazioni

Le carte del Level 5 devono essere primitive. Niente ali, fuoco o carapace enorme.

- **Morso Rinforzato**: piu' danno melee e leggero lifesteal su nemici piccoli. Tag: Predatore, Divoratore.
- **Nervi Iperattivi**: piu' velocita' e scatto piu' frequente, ma meno HP massimi. Tag: Larvale, Predatore.
- **Ghiandole Acide Primitive**: i morsi applicano corrosione leggera. Tag: Veleno, Alieno.
- **Zampe Supplementari**: piu' controllo movimento, meno rallentamento nelle melme, migliore raccolta biomassa. Tag: Insettoide, Larvale.
- **Pelle Adattiva**: breve riduzione danni dopo aver subito un colpo. Tag: Alieno, Corazzato leggero.

### Evento Speciale: La Prima Tentazione

A meta' livello il player trova una stanza con una scala verso la superficie. Non e' l'uscita principale, ma una fessura da cui entra luce.

Prompt possibile:

> La superficie e' vicina.

Il player puo' provare a salire. Se lo fa, entra in una micro-area dove vede guardie e viene quasi sicuramente ucciso. Dopo la prima volta, questa scelta deve diventare opzionale e veloce per non rompere il ritmo.

### Obiettivo

Sopravvivere abbastanza da accumulare biomassa, scegliere le prime mutazioni e sconfiggere il Jailer Husk. Dopo l'elite, si apre la scala per il Dungeon Level 4.

## Dungeon Level 4: La Cripta Marcia

### Identita'

Il Level 4 e' il primo livello veramente oscuro. Qui non ci sono piu' tracce della superficie. L'aria e' umida, pesante, piena di muffa e decomposizione.

Il tema e':

**Morte recente e carne marcia.**

Questo livello sposta il gioco da sopravvivenza base a costruzione della build.

### Ambiente

- cripte;
- tombe aperte;
- bare rotte;
- acqua stagnante;
- funghi;
- cadaveri appesi;
- altari minori;
- muri coperti di muffa;
- ossa fuse nella pietra.

### Nemici

- **Ghoul affamati**: corrono verso il player.
- **Scheletri arcieri**: primi nemici a distanza.
- **Cadaveri gonfi**: esplodono quando muoiono.
- **Necrofagi**: divorano cadaveri e diventano piu' forti.
- **Spiriti deboli**: attraversano ostacoli lentamente.
- **Melme putride**: lasciano pozze tossiche.

Elite:

**Corpse Bell Ringer**, un non-morto che suona una campana arrugginita. Finche' e' vivo, attira piu' orde.

### Gameplay

Qui il player impara a gestire priorita':

- gli arcieri vanno eliminati;
- i cadaveri esplosivi vanno evitati;
- il Bell Ringer va ucciso subito;
- restare fermi e' pericoloso.

### Mutazioni Introdotte

- **Artigli Primari**: attacco melee piu' largo e sanguinamento leggero. Tag: Predatore, Bestia.
- **Coda Grezza**: colpo automatico dietro al player. Tag: Rettiliano, Bestia.
- **Sangue Acido**: quando subisci danno, schizzi acido vicino. Tag: Veleno, Alieno, Abominio.
- **Stomaco Divorante**: la biomassa cura di piu', con bonus su uccisioni rapide. Tag: Divoratore, Abominio.

### Obiettivo

Sopravvivere alla cripta, sconfiggere il Corpse Bell Ringer e scendere al Dungeon Level 3.

Il livello dovrebbe chiudersi con questa sensazione:

> Non sono piu' solo una larva. Sto diventando qualcosa.

## Dungeon Level 3: Il Nido Antico

### Identita'

Il Level 3 e' biologico. Qui il dungeon non sembra piu' solo pietra. Sembra vivo.

Il tema e':

**Nido, sciame, predazione.**

Qui le build iniziano davvero a differenziarsi.

### Ambiente

- tunnel organici;
- uova;
- ragnatele;
- bozzoli;
- funghi luminosi;
- carcasse mezze digerite;
- pareti pulsanti;
- crepe piene di insetti;
- camere circolari da arena.

### Nemici

- **Ragni giganti**: lanciano ragnatele che rallentano.
- **Vermi corazzati**: caricano in linea retta.
- **Sciami di insetti**: tanti nemici piccoli.
- **Bestie cieche**: reagiscono al movimento.
- **Funghi esplosivi**: rilasciano spore.
- **Larve predatrici**: versioni naturali e ostili del player.

Elite:

**Brood Sentinel**, una creatura insettoide che protegge le uova del nido e puo' evocare sciami.

### Gameplay

Questo e' il livello anti-pigrizia. Il player deve muoversi bene.

Problemi principali:

- orde dense;
- rallentamenti;
- nemici che caricano;
- spazi piu' stretti;
- trappole naturali;
- sciami che puniscono build senza danno ad area.

Le build veloci si divertono. Le build lente devono avere carapace, spine o danno ad area.

### Mutazioni Introdotte

- **Gambe Predatorie**: piu' velocita', dash migliore, bonus danno dopo uno scatto. Tag: Predatore, Bestia.
- **Gambe Multiple**: piu' stabilita', meno rallentamento, migliore controllo, bonus contro knockback. Tag: Insettoide, Corazzato.
- **Carapace Leggero**: riduzione danni e lieve riduzione velocita'. Tag: Corazzato, Insettoide.
- **Ghiandole Larvali**: evoca minion deboli che raccolgono biomassa o attaccano. Tag: Brood, Insettoide, Divoratore.

### Obiettivo

Sconfiggere il Brood Sentinel e divorare il cuore del nido.

Qui il player puo' iniziare a sbloccare archetipi come:

- Reaper;
- Broodmother;
- Behemoth;
- Plague Horror;
- Manticora embrionale.

## Dungeon Level 2: Le Sale Del Culto

### Identita'

Il Level 2 e' dove il dungeon smette di essere naturale e diventa intenzionale. Qualcuno ha studiato, sigillato o venerato le cose del profondo.

Il tema e':

**Magia, rituale, controllo.**

Qui compaiono nemici piu' intelligenti.

### Ambiente

- sale rituali;
- cerchi magici;
- statue spezzate;
- libri proibiti;
- prigioni sacrificali;
- cristalli oscuri;
- canali di sangue;
- portali instabili;
- grandi stanze con altari.

### Nemici

- **Cultisti**: deboli ma numerosi.
- **Acoliti del sangue**: curano o buffano altri nemici.
- **Necromanti**: evocano scheletri.
- **Maghi oscuri**: lanciano proiettili o zone magiche.
- **Golem di carne**: tank lenti.
- **Guardie possedute**: umani mutati, piu' forti dei soldati normali.
- **Occhi rituali**: torrette magiche statiche.

Elite:

**The Choir of Flesh**, un gruppo di cultisti fusi insieme in un corpo solo. Canta, evoca, rallenta e distorce l'arena.

### Gameplay

Qui il gioco diventa horde con target prioritari.

Il player deve:

- evitare zone magiche;
- interrompere caster;
- uccidere healer;
- non farsi bloccare dai golem;
- distruggere cristalli rituali.

Una build solo melee senza mobilita' soffre. Una build solo veleno puo' faticare contro costrutti e non-morti. Una build psichica puo' controllare cultisti, ma non i boss.

### Mutazioni Introdotte

- **Nucleo Psichico**: chance di confondere nemici deboli, aura di paura, bonus contro umani vivi. Tag: Psichico, Alieno.
- **Tentacoli Neurali**: attaccano automaticamente nemici vicini e possono interrompere caster. Tag: Alieno, Abisso, Psichico.
- **Urlo Mentale**: onda che spinge o stordisce nemici, piu' efficace contro umani e bestie. Tag: Psichico, Abisso.
- **Polmone Termico**: prepara l'accesso a sputo di fuoco, resistenza a freddo/magia oscura, piccolo danno bruciante vicino al corpo. Tag: Fuoco, Drago.

### Obiettivo

Distruggere il rituale e sconfiggere The Choir of Flesh.

Dopo questo livello, il player dovrebbe essere quasi una creatura adulta. Qui possono sbloccarsi archetipi importanti:

- Mind Parasite;
- Abyssal Horror;
- Drago Alieno;
- Plague Horror;
- Behemoth avanzato.

## Dungeon Level 1: Il Cuore Sepolto

### Identita'

Questo e' il piano piu' profondo del dungeon. Non e' piu' un dungeon normale: e' qualcosa di antico, sigillato e quasi cosmico.

Il tema e':

**Origine, sigillo, mostruosita' antica.**

Qui la creatura aliena trova qualcosa che somiglia a lei.

### Ambiente

- pietra nera;
- ossa enormi;
- radici morte;
- cristalli organici;
- architettura impossibile;
- resti di creature antiche;
- porte sigillate;
- un enorme nucleo centrale;
- stanze che sembrano interne a un organismo.

Il dungeon non sembra costruito da umani. Sembra costruito per contenere qualcosa.

### Nemici

- **Cavalieri morti antichi**: lenti ma pesanti.
- **Guardiani ossei**: tank con scudi.
- **Demoni minori**: veloci e aggressivi.
- **Costrutti del sigillo**: resistenti a veleno e psichico.
- **Abomini falliti**: creature mutate male.
- **Ossa giganti animate**: nemici enormi ma lenti.
- **Sentinelle del nucleo**: proteggono il boss.

Elite:

**Failed Ascendant**, una creatura simile al player ma incompleta. Potrebbe essere una precedente larva aliena o un esperimento fallito.

### Gameplay

Questo e' il test finale della fase dungeon.

Il livello chiede:

- hai abbastanza danno?
- hai abbastanza mobilita'?
- hai una risposta ai nemici corazzati?
- hai sustain?
- hai una build coerente?

Una build casuale deve soffrire. Il roguelike deve punire il caos totale, senza diventare ingiusto.

### Mutazioni Introdotte

- **Ali Membranose**: dash lungo, salto sopra orde, accesso ad archetipi Drago/Wyvern. Tag: Volante, Drago.
- **Sputo di Fuoco**: cono di fuoco, danno ad area, panico sui nemici vivi. Tag: Fuoco, Drago.
- **Carapace Pesante**: grande riduzione danni, piu' HP, meno velocita', hitbox piu' grande. Tag: Corazzato, Behemoth.
- **Massa Instabile**: piu' danno e HP, tentacoli/spine casuali, meno controllo. Tag: Abominio, Caos, Alieno.

### Boss: The Buried Warden

The Buried Warden e' il Guardiano Sepolto. Non e' solo un mostro grosso: e' il motivo per cui il dungeon esiste.

Il dungeon e' stato costruito sopra un sigillo. Il Guardiano impedisce alle cose profonde di risalire in superficie. Forse secoli prima un'altra creatura aliena era caduta nel mondo e aveva quasi distrutto il regno.

Il player non e' il primo.

Fase 1:

- colpi pesanti;
- evocazione di ossa;
- barriere;
- onde d'urto.

Fase 2:

- il terreno si rompe;
- mani ossee escono dal pavimento;
- cristalli del sigillo sparano energia;
- il boss diventa piu' aggressivo.

Fase 3:

- il nucleo del boss si apre;
- il player deve attaccarlo e divorarlo.

Ricompensa:

**Nucleo Ascendente**

Effetto narrativo:

- la creatura e' completa;
- il dungeon non puo' piu' trattenerla;
- si apre una scala diretta verso la superficie.

Effetto gameplay:

- scelta di una mutazione finale;
- sblocco della classe primaria;
- bonus forte in base all'archetipo raggiunto.

Esempi:

- **Drago Alieno**: fuoco potenziato e ali stabili.
- **Behemoth**: armatura pesante e rigenerazione.
- **Reaper**: dash reset su kill.
- **Broodmother**: larve piu' forti.
- **Mind Parasite**: paura migliorata contro umani.
- **Plague Horror**: cadaveri contaminati.

## Surface Level 6: I Campi Esterni

### Identita'

Dopo il boss del dungeon, il player emerge in superficie.

Questo livello e' un payoff. Prima il player aveva paura della superficie. Ora la superficie ha paura del player.

Il tema e':

**La prima invasione.**

### Ambiente

- campi coltivati;
- fienili;
- mulini;
- staccionate;
- animali che scappano;
- contadini terrorizzati;
- torce;
- campane d'allarme in lontananza;
- sentieri fangosi.

### Nemici

- **Contadini armati**: forconi, asce, zappe.
- **Cani da guardia**: veloci, inseguono.
- **Cacciatori**: usano archi deboli.
- **Guardie rurali**: prime unita' organizzate.
- **Preti locali**: piccoli scudi o cure.
- **Milizia improvvisata**: gruppi numerosi ma deboli.

Elite:

**Village Hunter**, un cacciatore esperto che prova a rallentare il player con trappole e frecce.

### Gameplay

Il player deve sentirsi forte. Molti nemici muoiono facilmente, ma il livello non deve essere gratis.

Problemi introdotti:

- campane che chiamano rinforzi;
- cani che inseguono;
- trappole nei campi;
- arcieri distanti.

### Obiettivo

Distruggere le campane o sopravvivere all'allarme, poi avanzare verso il villaggio.

Il livello deve dire:

> Ora sei il mostro.

## Surface Level 7: Il Villaggio

### Identita'

Questo e' il primo livello umano vero.

Il tema e':

**Panico organizzato.**

Il villaggio non e' preparato per una creatura aliena, ma prova a difendersi.

### Ambiente

- case di legno e pietra;
- strade strette;
- piazza centrale;
- pozzo;
- chiesa;
- mercato;
- barricate improvvisate;
- tetti con arcieri;
- civili che scappano.

### Nemici

- **Villici armati**: tanti e deboli.
- **Arcieri sui tetti**: fastidiosi, da raggiungere o evitare.
- **Guardie cittadine**: scudo e spada.
- **Preti**: curano o respingono.
- **Portatori di torce**: creano zone di fuoco.
- **Milizia con reti**: rallenta build veloci.

Elite:

**The Bell Captain**, il capitano della milizia. Se resta vivo, continua a chiamare rinforzi.

### Gameplay

Qui entrano:

- strettoie;
- barricate;
- nemici sui tetti;
- fuoco;
- campane;
- piccoli obiettivi secondari.

Obiettivi possibili:

- distruggi la campana;
- abbatti la barricata;
- divora il capitano;
- sopravvivi alla milizia.

### Mutazioni Post-Dungeon

- **Terrore Biologico**: i nemici deboli possono fuggire. Tag: Psichico, Abominio.
- **Fame dei Vivi**: divorare umani cura piu' della biomassa non-morta. Tag: Divoratore.
- **Pelle Anti-Fiamma**: riduce danni da torce e fuoco. Tag: Corazzato, Drago.

### Obiettivo

Distruggere la resistenza del villaggio e raggiungere la caserma.

Nel Level 6 gli umani fuggono. Nel Level 7 iniziano a difendersi.

## Surface Level 8: Caserma E Mura Interne

### Identita'

Qui gli umani smettono di essere prede disordinate. Diventano un esercito.

Il tema e':

**Adattamento umano.**

Il regno capisce che la creatura non e' un animale normale.

### Ambiente

- caserma;
- mura;
- cortili;
- torri;
- armeria;
- stalle;
- ponte levatoio;
- trappole;
- bracieri;
- zone fortificate.

### Nemici

- **Soldati con scudo**: bloccano attacchi frontali.
- **Lancieri**: fermano cariche e Behemoth.
- **Balestrieri**: anti-volo.
- **Alchimisti**: lanciano bombe acide o incendiarie.
- **Trappolieri**: reti e catene.
- **Sergenti**: buffano soldati vicini.
- **Cavalieri leggeri**: caricano e scappano.

Elite:

**The Alchemist Marshal**, un comandante alchimista che studia la creatura e usa bombe specifiche contro rigenerazione, veleno o carapace.

### Gameplay

Questo e' il primo livello con counter-build seri.

Esempi:

- se sei molto grande, i lancieri ti puniscono;
- se voli, i balestrieri ti buttano giu';
- se rigeneri troppo, gli alchimisti riducono la cura;
- se usi minion, i bracieri li bruciano;
- se sei veloce, reti e catene ti bloccano.

Il player deve adattarsi o pagare le scelte sbagliate.

### Obiettivi

- distruggere le torri;
- abbattere il portone;
- divorare il comandante;
- sopravvivere all'assalto organizzato.

### Ricompense

- **Scaglie Anti-Proiettile**: riduce danno da frecce e balestre.
- **Carica Spezzascudi**: rompe scudi e formazioni.
- **Sangue Corrosivo Avanzato**: danneggia armature e lance.
- **Ossa Leggere**: riduce la penalita' di velocita' per forme grandi.

## Surface Level 9: La Fortezza Del Lord

### Identita'

Questo e' il livello prima del boss finale. Non e' piu' panico. E' guerra.

Il tema e':

**L'ultima difesa del regno.**

### Ambiente

- fortezza;
- cortili interni;
- mura alte;
- cappella militare;
- sala del trono;
- armeria nobile;
- ponti stretti;
- statue sacre;
- stendardi;
- fuoco, fumo, urla.

### Nemici

- **Cavalieri pesanti**: armatura alta, danno alto.
- **Paladini**: danno sacro, anti-abominio.
- **Maghi di corte**: zone magiche e barriere.
- **Capitani reali**: comandano formazioni.
- **Guardie con alabarde**: controllo area.
- **Balestrieri elite**: alta precisione.
- **Sacerdoti da guerra**: cure, scudi, esorcismi.
- **Cavalleria interna**: cariche in corridoi larghi.

Elite:

**The Lord's Executioner**, un boia corazzato enorme, lento ma devastante. Usa una grande ascia o una lama da esecuzione.

### Gameplay

Questo e' il controllo finale prima del boss.

Il gioco verifica:

- danno contro armatura;
- capacita' di gestire range;
- risposta ai caster;
- sustain;
- mobilita';
- gestione degli elite;
- coerenza dell'archetipo.

Il livello deve essere duro. Non deve sembrare ingiusto, ma deve dire chiaramente:

> Se la tua build e' trash, qui finisce.

### Obiettivi

- rompere il cancello interno;
- distruggere i sigilli sacri;
- uccidere l'Executioner;
- raggiungere la piazza del duello finale.

### Ricompensa Finale Prima Del Boss

Dopo il Level 9, il player ottiene una scelta finale:

**Mutazione Apex**

Esempi:

- **Drago Alieno - Cuore di Plasma**: il fuoco diventa plasma biologico, brucia armatura e lascia scie sul terreno.
- **Behemoth - Massa Regale**: piu' nemici sono vicini, piu' aumentano armatura e danno da contatto.
- **Reaper - Istinto Assoluto**: ogni kill resetta parzialmente il dash e aumenta critico per pochi secondi.
- **Broodmother - Nido Vivente**: genera larve piu' forti e puo' sacrificare minion per curarsi.
- **Mind Parasite - Corona Neurale**: i nemici umani deboli possono diventare alleati temporanei.
- **Plague Horror - Pestilenza Perfetta**: i cadaveri esplodono in nube tossica e propagano infezione.

## Surface Level 10: Il Campione Del Regno

### Identita'

Questo e' il boss finale.

Il tema e':

**Mostro contro campione.**

Il player entra in una grande arena davanti alla fortezza o nella piazza reale. Tutto brucia. Il villaggio e' distrutto. I soldati si ritirano. Le campane suonano.

Poi arriva il campione.

### Boss: Sir Aldric, The Radiant Lance

Sir Aldric e' un cavaliere sacro su cavallo da guerra. E' il campione del regno, mandato a fermare la creatura.

Arena:

- grande piazza;
- statue sacre;
- fuoco ai lati;
- soldati feriti;
- stendardi spezzati;
- zone di luce sacra;
- spazio sufficiente per cariche a cavallo.

### Fase 1: Cavaliere A Cavallo

Aldric combatte mantenendo distanza.

Attacchi:

- carica con lancia;
- affondo frontale;
- giro largo del cavallo;
- chiamata di arcieri;
- scia sacra lasciata dalla lancia;
- scudo di luce temporaneo.

Contro-build:

- punisce creature lente;
- punisce chi attacca frontalmente senza timing;
- forza il player a schivare.

### Fase 2: Il Giuramento Sacro

Quando perde abbastanza vita, Aldric usa magia sacra.

L'arena cambia:

- appaiono zone luminose che bruciano biomassa;
- i soldati pregano ai bordi;
- la lancia ottiene attacchi ad area;
- il cavallo diventa piu' aggressivo.

Attacchi:

- onda sacra;
- lancia lanciata e richiamata;
- carica doppia;
- esplosioni di luce dove il player resta troppo fermo.

### Fase 3: Fusione Con Il Cavallo

Il cavallo viene ferito. Aldric sta perdendo. Per salvare il regno, sacrifica se stesso e la cavalcatura.

La magia sacra li fonde in un centauro corazzato mostruoso.

A questo punto il boss diventa uno specchio del player:

- il player e' mutazione aliena;
- Aldric e' mutazione sacra.

Attacchi finali:

- cariche piu' brevi ma piu' rapide;
- colpi di lancia ad area;
- zoccoli che creano onde d'urto;
- scudo frontale;
- luce sacra che riduce rigenerazione;
- evocazione finale di paladini fantasma.

### Vittoria

Quando il player lo sconfigge, puo' divorare il cuore sacro del cavaliere.

Finale possibile:

> Ora la luce non brucia piu'.  
> Ora la carne armata cade come tutte le altre.  
> Questo mondo ha imparato il mio nome senza che io lo pronunciassi.  
> Non sono piu' la cosa sotto il dungeon.  
> Sono cio' che sale.

## Nota Di Design Critica

Il Dungeon Level 5 deve essere il piu' curato di tutti, perche' e' dove il player capisce il gioco.

Deve comunicare tre cose nei primi minuti:

1. La superficie esiste ed e' l'obiettivo.
2. Se ci vai ora, muori.
3. Devi scendere per diventare qualcosa di peggiore.

Se il player capisce questo nei primi 3 minuti, il concept funziona.

Se invece il Level 5 sembra solo un primo livello con scheletri, il gioco perde identita'. Il player non sta scappando dal dungeon. Sta usando il dungeon come stomaco evolutivo prima di risalire.
