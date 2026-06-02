# Dungeon Brood - Game Design e Vertical Slices

## Obiettivo del documento

Questo documento definisce cosa deve contenere Dungeon Brood e in quale ordine costruirlo. Il progetto deve restare C++ first, con asset e Blueprint introdotti solo quando servono a rendere giocabile e leggibile la vertical slice.

Stato attuale: infrastruttura test, Slice 0 e Slice 1 sono completate. Il gioco parte in finestra, genera una arena semplice, mostra placeholder distinti per player e nemici e comunica hit, danni e morte. La prossima priorita e rendere fisica la biomassa tramite drop raccoglibili e verificare la pressione offensiva delle wave.

## Visione

Dungeon Brood e un action roguelite single-player in terza persona dove il giocatore controlla una creatura appena nata in un dungeon. Ogni stanza e un combattimento breve. Uccidendo nemici si ottiene biomassa. A fine stanza si sceglie una mutazione che cambia statistiche, comportamento o capacita. La run deve durare poco, essere leggibile e spingere a provare build diverse.

## Pilastri

- Mostro in evoluzione: ogni scelta deve far sentire la creatura piu diversa, non solo piu forte.
- Combattimento leggibile: player, nemici, hit range, danni, morte e ricompense devono essere chiari anche con asset placeholder.
- Run compatta: poche stanze, ritmo alto, feedback immediato.
- C++ first: gameplay core, spawn, stats, evoluzioni e regole della run vivono in C++.
- Asset leggeri: fino alla vertical slice stabile usare mesh, materiali e componenti semplici creati o assegnati da codice.

## Target di esperienza

Il giocatore deve capire entro 10 secondi:

- quale creatura controlla;
- dove sono i nemici;
- quando un attacco colpisce;
- quanta vita e stamina ha;
- quando una stanza e conclusa;
- quale mutazione sta scegliendo;
- se la run e vinta o persa.

## Loop principale

1. Il giocatore entra o viene spawnato in una stanza.
2. La stanza si chiude e spawna un gruppo di nemici.
3. Il giocatore combatte usando movimento, attacco base e dodge.
4. I nemici morti rilasciano pickup biomassa.
5. Quando tutti i nemici sono morti, il gioco propone 3 evoluzioni.
6. Il giocatore sceglie una evoluzione.
7. Si passa alla stanza successiva o a una wave successiva.
8. Dopo il boss finale la run termina con vittoria.
9. Se il player muore, la run termina con sconfitta e puo essere riavviata.

## Contenuti richiesti

### Player

Il player deve avere:

- rappresentazione visibile, anche placeholder, diversa dai nemici;
- camera third-person stabile;
- movimento WASD;
- rotazione/look con mouse;
- attacco melee frontale;
- dodge con consumo stamina e cooldown;
- health, stamina, biomass;
- feedback di danno ricevuto;
- feedback di morte;
- supporto a evoluzioni che modificano stats e capacita.

Placeholder accettabile:

- capsula o mesh semplice con materiale verde/scuro;
- freccia o componente visivo per indicare fronte/direzione;
- debug shape breve per attacco.

### Nemici

Ogni nemico deve avere:

- rappresentazione visibile distinta per tipo;
- health;
- comportamento base di inseguimento;
- attacco con range e cooldown;
- morte visibile;
- biomassa reward.

Archetipi iniziali:

- Melee Grub: debole, veloce, corpo a corpo.
- Spitter: medio, mantiene piu distanza, attacco ranged o placeholder ranged leggibile.
- Brute: lento, resistente, colpo forte.
- Boss Brute: versione finale piu grande e minacciosa.

Placeholder accettabile:

- capsule/cubi/sfere colorate per tipo;
- scala diversa per brute e boss;
- materiali rossi/arancio/viola;
- testo/debug sopra il nemico solo se utile nella fase iniziale.

### Combat

Il combat minimo deve includere:

- attacco player con area leggibile;
- danno applicato una sola volta per bersaglio per swing;
- dodge direzionale;
- stamina regen;
- nemici che colpiscono il player;
- morte player e morte nemico;
- hit feedback tramite colore, log o debug message.

Da evitare per ora:

- combo lunghe;
- animazioni complesse;
- lock-on;
- projectile system avanzato;
- ability system completo se non usato dalla slice.

### Evoluzioni

Le evoluzioni devono essere scelte tra 3 opzioni dopo ogni stanza/wave. Ogni evoluzione deve avere:

- id stabile;
- nome;
- descrizione chiara;
- effetto gameplay immediato;
- possibilita di essere testata senza asset extra.

Catalogo iniziale:

- Hardened Carapace: +Armor, -MovementSpeed.
- Rending Claws: +AttackPower.
- Neural Overdrive: +MovementSpeed, +MaxStamina.
- Digest Essence: heal on kill.
- Acid Blood: danno vicino quando il player viene colpito.
- Brood Instinct: +biomass reward.
- Venom Strike: bonus damage sull'attacco base.
- Predator Leap: dodge piu lungo e cooldown minore.
- Thick Hide: +MaxHealth.

Futuro catalogo desiderato:

- Mutazioni offensive che cambiano range, forma o target dell'attacco.
- Mutazioni difensive che cambiano armor, regen o mitigazione.
- Mutazioni di mobilita che cambiano dodge, sprint o leap.
- Mutazioni metaboliche che cambiano biomassa, heal o rischio/ricompensa.

### Stanze e run

La prima versione deve usare stanze create da codice, non una grande procedural dungeon. Deve contenere:

- arena leggibile;
- muri o limiti chiari;
- spawn point player;
- spawn point nemici;
- 3 encounter normali;
- 1 encounter boss;
- fine run con vittoria.

In seguito:

- piu layout di stanza;
- porte;
- transizione stanza-stanza;
- scelta percorso semplice;
- proceduralita leggera solo quando il loop base e solido.

### UI e feedback

UI minima richiesta:

- health;
- stamina;
- biomass;
- wave/stanza corrente;
- nemici rimasti;
- messaggio scelta evoluzione;
- messaggio morte;
- messaggio run complete.

Per la fase iniziale sono accettabili on-screen debug messages, ma la UI deve diventare un HUD semplice appena combat e loop sono stabili.

### Audio e VFX

Rimandati fino a quando la slice visiva e giocabile. Quando introdotti:

- hit sound;
- death sound;
- evolution selected sound;
- simple impact effect;
- simple enemy death effect.

### Asset

Politica iniziale:

- usare mesh engine built-in o componenti primitivi;
- creare materiali semplici da codice quando possibile;
- preferire asset gratuiti Epic/Fab con licenza verificata e compatibilita Unreal Engine;
- mantenere fallback placeholder code-only per build e smoke test headless;
- isolare riferimenti agli asset in punti sostituibili, senza legare il gameplay core a un pack specifico;
- evitare asset binari grandi finche il gameplay non e stabile.

Asset minimi da introdurre presto:

- player placeholder visibile;
- enemy placeholder per ogni tipo;
- materiale arena/floor/wall;
- marker o decal semplice per attack range se necessario.

Direzione asset gratuita:

- cercare in Fab con filtro `Price > Free` e compatibilita Unreal Engine;
- valutare come prima fonte i contenuti gratuiti Epic Games;
- candidato iniziale per i nemici: `Paragon: Minions`, gratuito, con modelli, animazioni e FX utilizzabili nei progetti Unreal Engine;
- usare asset terzi gratuiti solo dopo verifica licenza, compatibilita UE 5.7, skeleton e costo di integrazione;
- non usare il nome o trademark `PARAGON` nel nome o nella promozione del gioco.

## Architettura gameplay desiderata

### Classi principali

- `ABroodGameMode`: gestisce run, wave, spawn, scelta evoluzioni e vittoria/sconfitta.
- `ABroodCharacter`: player, input, stats base, attacco, dodge, applicazione evoluzioni.
- `ABroodEnemyCharacter`: nemico base con archetipi, AI semplice, danno e morte.
- `UBroodAttributeSet`: health, stamina, attack, armor, movement, biomass.
- `UEvolutionComponent`: possesso e validazione evoluzioni.
- `ARoomEncounterManager`: gestione stanza/encounter quando si passera da wave hardcoded a stanze.

### Direzione futura

Quando la vertical slice base funziona:

- separare definizione evoluzioni in data asset o catalogo dati;
- separare enemy archetype stats in data table/data asset;
- introdurre un manager stanze invece di wave hardcoded;
- introdurre HUD dedicato;
- introdurre un sistema leggero di combat feedback.

## Vertical slices

### Infra - Controllo e test manuale

Stato: in corso.

Obiettivo: poter controllare, mettere in pausa e chiudere il gioco senza Alt+F4, e avere un primo smoke test eseguibile da shell.

Implementato:

- `ABroodPlayerController` dedicato al controllo player/UI;
- menu ESC C++ con Resume, Restart Run, Exit Game;
- input `Escape` mappato a `TogglePauseMenu`;
- comandi console `BroodQuit` e `BroodRestart`;
- comando console `BroodControlSmokeTest` per pilotare movimento, attacco e dodge del pawn;
- `SmokeTestGame.bat` per avviare il gioco in modalita unattended/null RHI, eseguire il test controllo e chiuderlo tramite comando;
- `RunGame.bat` configurato per apertura finestra `1280x720`;
- build verificata;
- smoke test verificato: menu pausa creato e chiuso, pawn mosso di `98.7` unita, attacco e dodge invocati, uscita pulita;
- avvio normale `RunGame.bat` verificato con finestra reale e flag `-windowed -ResX=1280 -ResY=720`.

Verifica finale eseguita il 2 giugno 2026:

- `BuildGame.bat`: riuscito;
- `SmokeTestGame.bat`: riuscito;
- `SmokeTestWindowed.ps1`: finestra reale verificata, click sinistro attacca, ESC apre e richiude il menu;
- nessun processo Unreal di test rimasto aperto.

### Infra - Mira mouse indipendente

Stato: in corso, seconda passata asset/world implementata e verificata su UE 5.7.

Obiettivo: consentire al player di muoversi e scappare con WASD mentre mira e attacca in una direzione indipendente tramite mouse.

Implementato:

- camera fissa inclinata adatta al loop action roguelite;
- movimento WASD relativo alla camera e non alla direzione in cui guarda il personaggio;
- rotazione automatica del personaggio verso la posizione del cursore proiettata sul piano dell'arena;
- cursore gameplay visibile e disponibile anche dopo Resume dal menu pausa;
- attacco melee con click sinistro orientato verso il cursore, mantenendo Space come input alternativo;
- orbit camera con trascinamento tasto destro e pitch limitato;
- dodge orientato secondo l'ultimo input di movimento, non secondo la mira;
- rimosso il vecchio controllo camera tramite assi MouseX e MouseY;
- comando console `BroodMouseAimSmokeTest` e script `SmokeTestMouseAim.bat`;
- `SmokeTestWindowed.ps1` esteso per inviare un vero click sinistro alla finestra di gioco.

Verifica finale eseguita il 2 giugno 2026:

- `BuildGame.bat`: riuscito;
- `SmokeTestMouseAim.bat`: riuscito, pawn mosso di `540.6` unita mantenendo una mira indipendente laterale con dot product `1.00` e orbit camera di `45.0` gradi;
- `SmokeTestAll.bat`: riuscito end-to-end;
- `SmokeTestWindowed.ps1`: finestra reale `1280x720`, click sinistro reale, apertura e chiusura menu ESC verificati tramite log;
- nessun processo Unreal di test rimasto aperto;
- verifica visuale manuale consigliata: confermare a schermo il framing della camera e la sensazione della mira col cursore.

### Slice 0 - Visibilita e avvio corretto

Stato: completata e verificata su UE 5.7.

Obiettivo: il gioco non deve sembrare vuoto.

Contenuto:

- player visibile con mesh/materiale placeholder;
- nemici visibili con colori/scale diverse;
- floor e muri con materiali distinti;
- camera posizionata in modo che player e primi nemici siano visibili;
- messaggio iniziale con controlli essenziali;
- build e run senza popup di compilazione.

Criteri di accettazione:

- entro 5 secondi si vedono player, arena e almeno un nemico;
- il player puo muoversi e attaccare;
- un nemico morto scompare o cambia stato visibile;
- non servono asset esterni.

Implementato:

- player placeholder con mesh corpo, indicatore frontale e nameplate;
- nemici placeholder con mesh, scale e nameplate diversi per tipo;
- brute e boss piu grandi e riconoscibili;
- arena con marker spawn e marker direzione nemici;
- messaggio iniziale con controlli essenziali;
- build `DungeonBroodEditor Win64 Development` riuscita;
- `RunGame.bat` avviato e rimasto vivo durante la verifica.

Verifica finale eseguita il 2 giugno 2026:

- build editor riuscita;
- avvio finestra riuscito;
- smoke controllo riuscito: menu pausa, movimento, attacco e dodge;
- copertura automatica: presenza runtime e controllo pawn;
- verifica visuale manuale richiesta: confermare a schermo leggibilita di colori, scale e nameplate placeholder.

### Slice 1 - Combat leggibile

Stato: completata e verificata su UE 5.7.

Obiettivo: colpire, essere colpiti e morire devono essere chiari.

Contenuto:

- feedback visivo quando il player attacca;
- feedback quando un nemico subisce danno;
- feedback quando il player subisce danno;
- nemici con comportamento diverso per tipo;
- morte nemico leggibile;
- morte player leggibile.

Criteri di accettazione:

- il giocatore capisce se un attacco e andato a segno;
- il numero di nemici rimasti si aggiorna;
- il dodge evita concretamente situazioni pericolose;
- ogni nemico ha silhouette o colore distinto.

Implementato:

- attacco player con linea e sfera debug, estensione visiva dell'indicatore frontale e colore hit/miss;
- flash rosso sul player quando subisce danno;
- flash bianco sui nemici quando subiscono danno;
- nameplate nemici con tipo e health corrente;
- marker nemico che pulsa quando il nemico attacca;
- stato `DEFEATED`, mesh scura e caduta visibile prima della rimozione del nemico;
- Spitter che arretra quando il player entra troppo vicino, mentre Grub, Brute e Boss inseguono;
- `SmokeTestCombat.bat` e comando `BroodCombatSmokeTest`;
- test combat verificato: 3 hit da `10.0`, morte nemico e reward di `8.0` biomass;
- build e smoke test controllo verificati.

Verifica finale eseguita il 2 giugno 2026:

- `SmokeTestCombat.bat`: riuscito;
- log verificato: 3 hit da `10.0`, health nemico `28 -> 18 -> 8 -> 0`;
- log verificato: morte nemico e reward `8.0` biomass;
- avvio finestra riuscito dopo le modifiche combat;
- copertura automatica: hit, damage, kill, biomass, input attack/dodge e toggle ESC;
- verifica visuale manuale richiesta: confermare a schermo flash, pulse marker e stato `DEFEATED`.

### Slice 2 - Pressione wave e raccolta biomassa

Stato: completata e verificata su UE 5.7.

Obiettivo: consolidare la base survival del loop prima di costruire progressione e stanze.

Contenuto:

- ogni wave spawna nemici ostili che cercano attivamente il player;
- Grub, Brute e Boss inseguono fino al range attacco;
- Spitter mantiene distanza utile e attacca da lontano;
- i nemici morti non assegnano piu biomassa direttamente;
- ogni nemico morto droppa uno o piu pickup biomassa visibili;
- il player deve passare vicino ai pickup per raccoglierli;
- pickup con valore diverso per archetipi e feedback raccolta;
- biomassa non raccolta resta nella arena fino al pickup o alla fine encounter secondo regola esplicita.

Criteri di accettazione:

- restando fermo, il player viene raggiunto e attaccato;
- uccidere un nemico genera un pickup visibile;
- la biomassa aumenta solo dopo raccolta;
- pickup raccolti scompaiono;
- reward diversi per archetipi restano coerenti.

Verifica finale richiesta prima di segnare la slice come completata:

- smoke test pressione wave: player fermo, nemico raggiunge e danneggia il player;
- smoke test drop: kill nemico, biomassa invariata prima della raccolta;
- smoke test pickup: player raccoglie drop e riceve il valore atteso;
- smoke test regressione: `SmokeTestAll.bat`;
- verifica visuale manuale di inseguimento, drop e raccolta.

Implementato:

- `ABiomassPickup` code-only con mesh placeholder verde, label valore e overlap raccolta;
- spawn differito pickup per inizializzare il valore prima di attivare overlap;
- drop biomassa alla morte nemico al posto del reward diretto;
- raccolta tramite prossimita player con messaggio `+BIOMASS`;
- `Brood Instinct` applicato al valore del drop e `Digest Essence` mantenuto come heal-on-kill;
- `AAIController` automatico per nemici spawned;
- movement input AI attivo per Grub, Brute, Boss e Spitter;
- `SmokeTestWavePressure.bat` e comando `BroodWavePressureSmokeTest`;
- smoke test irrigiditi: i batch falliscono se manca il marker runtime `PASSED`.

Verifica finale eseguita il 2 giugno 2026:

- `BuildGame.bat`: riuscito;
- `SmokeTestCombat.bat`: riuscito;
- log verificato: drop `8.0` biomass, biomassa invariata prima pickup, raccolta `8.0`;
- `SmokeTestWavePressure.bat`: riuscito;
- log verificato: player fermo raggiunto e danneggiato per `7.0`;
- `SmokeTestAll.bat`: riuscito end-to-end;
- verifica visuale manuale richiesta: confermare a schermo inseguimento, pickup verde, label e messaggio raccolta.

### Slice 3 - Upgrade visuale con asset gratuiti

Stato: completata e verificata su UE 5.7.

Obiettivo: sostituire gradualmente i placeholder con creature leggibili e coerenti senza compromettere il loop C++ first.

Contenuto:

- selezione di un pack gratuito Epic/Fab con licenza e compatibilita verificate;
- prima integrazione consigliata: `Paragon: Minions` per almeno un archetipo nemico;
- mesh e materiali visibili per Grub, Spitter, Brute e Boss, con fallback placeholder;
- animazioni minime: idle, locomotion, attack, hit e death quando disponibili;
- scala, colore o silhouette distinti per ogni archetipo;
- ambiente migliorato con materiali gratuiti leggeri o engine built-in.

Implementato nel primo incremento:

- dungeon dressing code-spawned con fasce pavimento, pilastri, contrafforti, bracieri illuminati, cristalli biomassa e detriti;
- props decorativi senza collisione per non alterare movimento e AI;
- marker runtime `BROOD_WORLD_DRESSING_READY`;
- catalogo asset esterni e percorso di import in `docs/ASSET_INTEGRATION.md`;
- pack ambiente selezionato: `Medieval Dungeon` di Infuse Studio;
- pack creature selezionato: `Paragon: Minions` di Epic Games;
- fallback engine built-in mantenuto se gli asset Fab mancano;
- mesh `MedievalDungeon` collegate per pavimento, pareti decorative, pilastri, bracieri, bare, gargoyle, skeleton props e detriti quando il pack e presente localmente;
- mesh `ParagonMinions` collegate per player e archetipi nemici quando il pack e presente localmente;
- animazioni minime collegate su idle, attacco e morte quando disponibili.

Implementato nel secondo incremento:

- collision floor e collision wall mantenuti per gameplay ma resi invisibili;
- marker visivi di test rimossi dalla stanza runtime;
- pavimento passato a moduli `SM_Crypt_Floor` del pack `Medieval Dungeon`;
- pareti perimetrali, doorway est/ovest e arco nord spawnati come moduli dungeon visibili;
- player temporaneo spostato da `Prime_Helix` alla linea `Buff_*` di `Paragon: Minions` per distinguere profili mutazione;
- profili visuali player attuali: `BaseBrood`, `ToxicBrood`, `InstinctBrood`, `ClawBrood`, `ArmoredBrood`, `LeapingBrood`;
- animazione attacco player collegata dove compatibile; profilo tossico lasciato senza attack override per evitare animazioni cross-skeleton fragili;
- smoke windowed reso piu robusto cercando una finestra visibile per PID Unreal e usando fallback su screen bounds quando Windows non restituisce il rettangolo finestra.

Checkpoint eseguito il 2 giugno 2026:

- `BuildGame.bat`: riuscito;
- `SmokeTestAll.bat`: riuscito end-to-end;
- log finestra verificato: marker `BROOD_WORLD_DRESSING_READY`;
- log finestra verificato: marker `BROOD_IMPORTED_WORLD_ASSETS_READY`;
- log finestra verificato: marker `BROOD_IMPORTED_PLAYER_ASSET_READY`;
- log finestra verificato: marker `BROOD_IMPORTED_ENEMY_ASSET_READY`;
- log finestra verificato: trascinamento tasto destro con yaw finale `-45.1`;
- screenshot runtime precedente: player Paragon visibile, asset dungeon visibili e HUD leggibile;
- seconda verifica automatica: `BuildGame.bat` riuscito dopo passaggio a `SM_Crypt_Floor`;
- seconda verifica automatica: `SmokeTestAll.bat` riuscito dopo correzione world/player/windowed smoke;
- nessun processo Unreal di test rimasto aperto;
- nota: la slice non e ancora completata perche il player usa ancora una linea temporanea `Paragon: Minions/Buff_*` e serve una verifica visuale manuale stabile del pavimento `SM_Crypt_Floor`, scala, luci e composizione stanza.

Lavoro rimanente prima di completare la slice:

- verificare manualmente in editor/game che `SM_Crypt_Floor` non legga piu come template/checker;
- rifinire luci, camera e densita props per far leggere la stanza come dungeon, non solo arena decorata;
- scegliere una linea visuale definitiva del player adatta alla fantasia `brood/evoluzione`;
- definire asset o varianti definitive per fasi/mutazioni del player, almeno base, armored, claws/attack, speed/leap e thick hide;
- ridurre o riorganizzare gli asset Fab usati per non dipendere dall'intero pack;
- verifica visuale manuale di player, nemici e mondo prima dello stato `completata`.

Criteri di accettazione:

- il gioco resta avviabile anche se un asset opzionale manca;
- ogni archetipo e riconoscibile rapidamente;
- animazioni mancanti degradano al placeholder senza rompere gameplay;
- build e smoke test headless restano stabili.

Verifica finale richiesta prima di segnare la slice come completata:

- licenza e fonte degli asset registrate nel repository;
- build e `SmokeTestAll.bat`;
- avvio finestra con tutti gli archetipi;
- verifica visuale manuale di mesh, materiali, scala e animazioni;
- controllo assenza di dipendenze marketplace a pagamento.

### Slice 4 - Carte evoluzione e micro-run breve

Stato: non iniziata.

Obiettivo: completare una micro-run con scelte evoluzione leggibili e significative.

Contenuto:

- 3 carte evoluzione dopo ogni wave normale;
- carte con nome, descrizione, categoria e indicazione effetto;
- selezione mouse e tasti `1`, `2`, `3`;
- pausa combattimento mentre le carte sono visibili;
- applicazione effetto immediata;
- HUD debug o semplice per stats;
- boss finale;
- victory e restart.

Criteri di accettazione:

- le carte sono leggibili senza usare log;
- una run completa dura pochi minuti;
- almeno 6 evoluzioni hanno effetto percepibile;
- scegliere evoluzioni diverse cambia il modo di giocare;
- il boss finale chiude la run.

Verifica finale richiesta prima di segnare la slice come completata:

- smoke test micro-run completa;
- selezione carte con mouse e tastiera;
- scelta e applicazione di almeno 6 evoluzioni;
- boss finale sconfitto;
- victory e restart verificati;
- verifica visuale manuale della leggibilita delle carte.

### Slice 5 - Stanze invece di sola arena

Stato: non iniziata.

Obiettivo: far sembrare il gioco un dungeon roguelite, non una arena singola.

Contenuto:

- 2-3 layout stanza code-spawned;
- transizione tra stanze;
- porte o barriere;
- encounter manager usato per chiusura/apertura stanza;
- spawn point dedicati.

Criteri di accettazione:

- il player percepisce progressione stanza-stanza;
- ogni stanza ha composizione nemici diversa;
- nessun grande procedural dungeon ancora.

Verifica finale richiesta prima di segnare la slice come completata:

- smoke test transizione tra almeno 2 stanze;
- porte/barriere aperte e chiuse secondo stato encounter;
- composizioni nemici diverse verificate;
- verifica visuale manuale della progressione stanza-stanza.

### Slice 6 - HUD e presentazione minima

Stato: non iniziata.

Obiettivo: sostituire debug messages principali con UI giocabile.

Contenuto:

- HUD health/stamina/biomass;
- indicatore wave/stanza;
- schermata run complete/death;
- feedback visivo piu pulito.

Criteri di accettazione:

- il gioco e comprensibile senza leggere log;
- debug messages restano solo per diagnostica.

Verifica finale richiesta prima di segnare la slice come completata:

- health, stamina, biomass e stato run aggiornati durante gameplay;
- schermate death e victory verificate;
- verifica visuale manuale senza dipendere dai log.

### Slice 7 - Profondita roguelite leggera

Stato: non iniziata.

Obiettivo: aumentare replayability senza sistemi enormi.

Contenuto:

- piu evoluzioni;
- piccola randomizzazione delle scelte;
- composizioni nemiche variabili;
- 2 boss/mini-boss o varianti;
- bilanciamento base.

Criteri di accettazione:

- due run consecutive non sono identiche;
- non vengono introdotti inventory, save system o multiplayer;
- le nuove feature rafforzano il loop principale.

Verifica finale richiesta prima di segnare la slice come completata:

- almeno 2 run automatiche completate;
- differenze tra run registrate;
- varianti boss/mini-boss verificate;
- controllo regressioni su suite precedente.

## Non-obiettivi per ora

- Multiplayer o replication.
- Inventory.
- Save system.
- Grande procedural dungeon.
- Asset pesanti, a pagamento o con dipendenze non verificate.
- Animazioni complesse.
- Build system custom.
- Sistemi speculativi non usati dal loop attuale.

## Priorita immediata

La prossima fase consigliata e Slice 3.

Task concreti:

1. Selezionare e importare un pack gratuito Epic/Fab compatibile UE 5.7.
2. Registrare fonte e licenza asset nel repository.
3. Integrare almeno un archetipo nemico con fallback placeholder.
4. Verificare mesh, materiali e animazioni disponibili.
5. Mantenere stabile la suite headless.
6. Verificare build e run.

Questa fase migliora la presentazione visiva mantenendo il gameplay core indipendente da asset opzionali.
