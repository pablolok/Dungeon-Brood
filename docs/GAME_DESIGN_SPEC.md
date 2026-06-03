# Deepborn - Game Design Spec

## Purpose

This document is the current game design source of truth. It replaces the earlier single-level village crypt direction.

Implementation order, engineering tasks, and verification live in `docs/CURRENT_IMPLEMENTATION_PLAN.md`. Asset sources and license notes live in `docs/ASSET_INTEGRATION.md`.

## Working Title

Preferred title: **Deepborn**.

Other candidates:

- Larva Ascendant
- The Thing Below
- Crown of Biomass

`Deepborn` is the strongest current title because it is short, readable, dark-fantasy compatible, and works for Steam or trailer use.

## High Concept

Deepborn is a single-player third-person horde roguelite about an alien larva that falls into an ancient dungeon beneath a medieval village. The creature is not ready to invade the surface. It must descend into the dungeon, devour monsters, absorb biomass, mutate its body, and build a coherent form. After defeating the buried dungeon boss, it rises back to the surface and attacks the village, fortress, and final human champion.

The core fantasy is not escape. It is preparation for invasion.

## Narrative Premise

During a storm, an organic alien fragment falls from the sky. It lands nearly dead, incomplete, and hungry inside an old dungeon under a medieval village. Above are humans, guards, knights, walls, bells, and a fortress. The larva understands that the surface means immediate death.

Below are bones, rot, buried magic, sealed monsters, and enough biomass to rebuild the body.

The player begins between two paths:

- a lit stairway upward toward the surface;
- a dark stairway downward into the dungeon.

The player may try the surface immediately. That choice should be playable, not a cutscene. The larva reaches a small surface area and is overwhelmed by guards, archers, a war dog, and a knight. Death returns the player to the starting chamber with a message such as:

> This body was not ready.

This teaches the central premise: descend first, become monster, then return.

## Opening Monologue Target

The tone should be close to:

> The light above smells of iron, warm blood, and fear.  
> There are living prey up there. Too many. Armed. Organized.  
> This body is still soft. Incomplete. Insufficient.  
> If I climb now, I die.  
> Below are old bones, rotten flesh, and buried magic.  
> I will descend. I will devour. I will rebuild this body.  
> And when I return to the surface, I will no longer be a larva.

## Run Structure

The complete run has 10 levels split into two mirrored halves.

### Phase 1: Descent

The player starts at the dungeon level closest to the surface and descends toward the ancient core.

- Dungeon Level 5: Upper Threshold
- Dungeon Level 4: Rotten Crypt
- Dungeon Level 3: Ancient Nest
- Dungeon Level 2: Cult Halls
- Dungeon Level 1: Buried Heart

The numbering represents physical depth, not normal videogame difficulty ordering. Level 5 is nearest the surface. Level 1 is deepest, oldest, and most dangerous.

### Phase 2: Ascent

After defeating the Dungeon Level 1 boss, an organic, ancient, or magical stairway opens directly back toward the surface.

- Surface Level 6: Outer Fields
- Surface Level 7: Village
- Surface Level 8: Barracks and Walls
- Surface Level 9: Fortress
- Surface Level 10: Final Boss

The emotional arc is:

1. descend because the surface is death;
2. mutate into a real monster;
3. return and make the surface fear you.

## Core Loop

1. Enter a level.
2. Survive waves of enemies.
3. Kill and devour biomass.
4. Gain mutation progress.
5. Choose mutation cards.
6. Change the creature's body and gameplay.
7. Push toward one or more archetypes.
8. Choose whether to evolve form or delay evolution.
9. Descend deeper until the buried boss dies.
10. Ascend and attack the surface.

Every meaningful choice should change the body, not only a stat number.

## Body-First Mutation Rule

Every mutation card must do at least one of these:

- add a visible body part;
- modify an existing body part;
- change posture or movement;
- unlock a new ability;
- push toward an archetype.

Bad card:

- `+5% damage`

Good card:

- `Serrated Claws`: two foreclaws grow from the body. Basic attacks now cause bleeding. If the player also has `Predatory Legs`, unlock a leap against bleeding enemies.

## Mutation Card Families

Cards belong to body slots and evolutionary tags.

### Body Slots

- Head
- Mouth
- Eyes
- Arms
- Legs
- Back
- Tail
- Torso
- Organs
- Blood
- Aura

### Card Types

#### Body Mutation

Adds or changes physical parts.

Examples:

- claws;
- tail;
- wings;
- carapace;
- tentacles;
- eyes;
- horns;
- legs.

#### Organ Mutation

Changes internal functions.

Examples:

- toxic gland;
- devouring stomach;
- double heart;
- acid blood;
- psychic core;
- thermal lung.

#### Instinct Mutation

Changes behavior, triggers, or active skills.

Examples:

- predatory instinct;
- wound fury;
- growing hunger;
- induced panic;
- automatic escape dash;
- execution.

#### Unstable Mutation

Strong cards with penalties.

Examples:

- uncontrolled mass;
- explosive blood;
- insatiable hunger;
- tumorous growth;
- broken symmetry.

## Mutation Tags

Each card gives one or more tags. Tags accumulate and unlock archetype traits.

Primary tags:

- Predator
- Beast
- Reptilian
- Insectoid
- Dragon
- Venom
- Fire
- Psychic
- Alien
- Abyss
- Armored
- Flying
- Devourer
- Plague
- Demonic

When a tag reaches a threshold, the player unlocks related trait cards. When a specific combination is complete, the player unlocks an archetype.

Example:

- `Serrated Claws`: Predator + Beast
- `Membrane Wings`: Flying + Dragon
- `Thermal Gland`: Fire + Dragon
- `Segmented Tail`: Reptilian + Beast

## Body Slot Conflict

The player must not be able to take every mutation at full strength. Some cards compete for body slots.

### Mouth Slot

Main specialization examples:

- acid spit;
- fire breath;
- devouring bite;
- psychic scream.

### Back Slot

Main specialization examples:

- wings;
- tentacles;
- spines;
- dorsal carapace.

Combinations can exist but should add penalties:

- wings + heavy carapace: slower wings;
- wings + tentacles: weaker aerial control;
- spines + carapace: strong tank identity but no true flight.

### Legs Slot

Main specialization examples:

- predatory legs;
- multiple insectoid legs;
- heavy behemoth limbs;
- serpentine body.

Each choice changes movement.

## Form Evolution

The player starts as **Larva**. Form evolution can happen multiple times, but it should not be a mandatory straight line. It depends on biomass, thresholds, and the player's choice to evolve or delay.

### Form 1: Larva

Small, fragile, fast, flexible.

Traits:

- small hitbox;
- high speed;
- low HP;
- low damage;
- excellent biomass collection;
- can slip through enemy pressure more easily;
- access to primitive mutation cards.

Typical cards:

- small legs;
- venom;
- mandibles;
- regeneration;
- elastic body;
- short dash;
- rapid digestion.

### Form 2: Predator

The first real combat body.

Traits:

- more HP;
- more damage;
- less speed than larva;
- medium hitbox;
- access to claws, tail, mandibles, spines, and light armor;
- can specialize into build direction.

### Form 3: Abomination

Large, heavy, dangerous, harder to correct.

Traits:

- high HP;
- area damage;
- large hitbox;
- harder to build for speed;
- access to heavy carapace, tentacles, large wings, fire, toxin aura, and psionics;
- loses access to some larval cards.

## Delaying Evolution

At each evolution threshold, the player may:

### Evolve

Advantages:

- more HP;
- more damage;
- access to rare and advanced cards;
- stronger archetype traits.

Disadvantages:

- larger hitbox;
- lower speed;
- speed upgrades become less efficient;
- some larval cards disappear.

### Delay Evolution

Advantages:

- high speed;
- better evasion;
- more control;
- exclusive larval cards;
- possible "small but lethal" build.

Disadvantages:

- fragile;
- lower direct damage;
- risk arriving at bosses without enough mass.

Larval-exclusive card examples:

- `Threadlike Body`: reduces hitbox and improves slipping through enemies.
- `Hyperactive Nerves`: major speed bonus, lower HP.
- `Pure Venom`: venom scales better, physical damage drops.
- `Rapid Digestion`: biomass heals faster.
- `Soft Adaptive Skin`: brief invulnerability after taking damage.
- `Escape Instinct`: automatic dash out when surrounded.

## Archetypes

The player does not pick a class from a menu. Class emerges from mutation tags and body slots.

The game may support one primary archetype and one secondary archetype.

### Alien Dragon

Requirements:

- Dragon 4+
- Flying 2+
- offensive Mouth mutation
- Tail or Claws

Gameplay:

- medium-large body;
- strong frontal damage;
- cone attacks;
- aerial dash;
- strong against compact hordes.

Weakness:

- large hitbox;
- vulnerable to archers and spears;
- high stamina/energy costs.

Visual fantasy: an alien imitation of a dragon, with wrong-looking membrane wings, segmented tail, and biological plasma instead of magic fire.

### Manticore

Requirements:

- stinger tail;
- claws;
- toxic glands;
- horns, mandibles, or similar head mutation.

Gameplay:

- aggressive melee;
- poison;
- tail strikes;
- strong against elites and knights.

Weakness:

- limited wide-area control without extra upgrades.

### Broodmother

Requirements:

- swollen abdomen;
- larval glands;
- carapace;
- psychic or venom support.

Gameplay:

- summons small larvae;
- controls the field;
- weaker in direct melee;
- wins through quantity.

Weakness:

- vulnerable if minions are cleared;
- bosses with area attacks are dangerous.

### Armored Behemoth

Requirements:

- heavy carapace;
- spines;
- devouring stomach;
- robust legs.

Gameplay:

- slow tank;
- contact damage;
- crushes small enemies;
- heals by eating.

Weakness:

- poor mobility;
- ranged enemies and mobile bosses become dangerous.

### Reaper

Requirements:

- long claws;
- predatory legs;
- multiple eyes;
- unstable blood or hyperactive nerves.

Gameplay:

- very fast;
- critical hits;
- dash resets;
- rapid kills.

Weakness:

- fragile;
- bad when trapped;
- high skill requirement.

### Plague Horror

Requirements:

- toxic glands;
- acid spit;
- infected blood;
- organic aura.

Gameplay:

- damage over time;
- area control;
- weakens hordes.

Weakness:

- slower against resistant dungeon bosses and undead.

### Mind Parasite

Requirements:

- psychic core;
- multiple eyes;
- neural tentacles;
- fragile or larval body.

Gameplay:

- fear;
- confusion;
- converts weak enemies;
- excels against humans on the surface.

Weakness:

- weak direct damage;
- undead and constructs resist control;
- bosses are mostly immune but can be debuffed.

### Abyssal Horror

Requirements:

- tentacles;
- multiple eyes;
- unstable blood;
- abyssal mutations.

Gameplay:

- area damage;
- automatic attacks;
- chaos and crowd pressure.

Weakness:

- less precise control;
- weak against ranged targets.

### Wyvern

Requirements:

- wings;
- tail;
- claws;
- light legs.

Gameplay:

- high mobility;
- diving attacks;
- hit-and-run.

Weakness:

- less HP than Alien Dragon;
- less frontal damage;
- must keep moving.

### Chimera

Requirements:

- mixed mutations with no dominant tag;
- at least four mutation families.

Gameplay:

- flexible hybrid;
- extra card access;
- reduces penalties from incompatible mutations.

Weakness:

- lower peak power than focused builds.

## Level Progression

### Dungeon Level 5: Upper Threshold

Player state: larva.

Enemies:

- weak skeletons;
- mutated rats;
- small slimes;
- bats;
- animated human remains.

Purpose:

- real tutorial;
- first cards;
- first survival, speed, damage, or venom choices.

### Dungeon Level 4: Rotten Crypt

Enemies:

- ghouls;
- skeleton archers;
- exploding corpses;
- corpse eaters;
- dungeon larvae.

Purpose:

- introduces corpses, venom, and aggressive enemies;
- first specialization pressure.

### Dungeon Level 3: Ancient Nest

Enemies:

- giant spiders;
- armored worms;
- blind beasts;
- swarms;
- sentient fungi.

Purpose:

- movement test;
- dense hordes;
- slows and traps.

### Dungeon Level 2: Cult Halls

Enemies:

- cultists;
- dark mages;
- necromancers;
- flesh golems;
- possessed guards.

Purpose:

- ranged pressure;
- magic zones;
- priority targets.

### Dungeon Level 1: Buried Heart

Enemies:

- ancient dead knights;
- bone abominations;
- minor demons;
- magical constructs;
- seal guardians.

Boss: **The Buried Warden**

The Buried Warden is an ancient guardian made of bone, black roots, and dead creatures. It is the seal that prevents things from below from rising.

Boss phases:

- Phase 1: slow heavy strikes, skeleton summons, shockwaves.
- Phase 2: cracks open the ground; bone hands punish stationary play.
- Phase 3: exposes its core; the player must devour it.

Reward: **Ascending Core**

Effects:

- opens the route back to the surface;
- grants one final mutation before the village;
- increases maximum body size.

## Surface Progression

### Surface Level 6: Outer Fields

Enemies:

- farmers;
- dogs;
- hunters;
- light guards;
- local priests.

Purpose:

- controlled power fantasy;
- humans begin adapting.

### Surface Level 7: Village

Enemies:

- armed villagers;
- rooftop archers;
- militia;
- priests;
- traps;
- fire.

Special mechanic:

- bells call reinforcements if not destroyed.

### Surface Level 8: Barracks and Walls

Enemies:

- soldiers;
- spearmen;
- crossbowmen;
- alchemists;
- squires;
- net units.

Purpose:

- anti-build counters.

Examples:

- nets counter fast builds;
- spears counter Behemoth charges;
- crossbows counter flight;
- alchemists counter regeneration;
- priests counter psychic/plague.

### Surface Level 9: Fortress

Enemies:

- knights;
- paladins;
- court mages;
- captains;
- artillery;
- light cavalry.

Purpose:

- final build test before the boss.

### Surface Level 10: Final Boss

Boss: **Sir Aldric, the Radiant Lance**

Sir Aldric is the kingdom's sacred champion. He is humanity's response to the creature.

Thematic mirror:

- the player is alien evolution;
- Aldric is sacred evolution.

Phases:

1. Mounted lance combat, charges, archers, distance control.
2. The horse is wounded; Aldric uses sacred magic and burning light zones.
3. Preferred final phase: Aldric sacrifices the horse and fuses with it through sacred magic, becoming an armored radiant centaur-like monster.

This makes the final boss a mutation too, but human and divine instead of alien and organic.

## Enemy Counter Design

Dungeon enemies:

- skeletons: basic and numerous;
- ghouls: fast chasers;
- slimes: slowing hazards;
- spiders: traps;
- necromancers: summoners;
- cultists: buffers;
- golems: tanks;
- guardians: elites.

Surface enemies:

- villagers: many and weak;
- dogs: chase fast builds;
- archers: punish slow or large builds;
- spearmen: stop charges;
- crossbowmen: anti-flight;
- alchemists: anti-regeneration;
- priests: anti-plague and anti-psychic;
- knights: anti-weak melee;
- paladins: anti-abomination.

Every build should have power moments and suffering moments.

## Visual Design

The player must see the body change.

Modular parts:

- head;
- mouth;
- eyes;
- arms;
- legs;
- back;
- tail;
- torso;
- organs or aura;
- general size.

Examples:

- claws lengthen the arms and alter the attack animation;
- multiple legs lower the body and change movement;
- wings change silhouette and dash feedback;
- carapace widens the body and changes hit feedback.

This must stay scoped. Use a limited number of body slots with clear visual changes, not unlimited freeform mutation.

## Current Prototype Relationship

The current C++ prototype already proves:

- third-person movement;
- mouse aim;
- melee attack;
- dodge;
- biomass drops;
- evolution choices;
- larva fallback visual;
- wave progression;
- boss completion smoke coverage.

However, the prototype's current village-crypt micro-run is now only a stepping stone. The new design target is the full descent/ascent structure with dungeon monsters first and human surface escalation later.

## Non-Goals For Now

- multiplayer or replication;
- inventory;
- save system;
- large procedural dungeon;
- asset-heavy production before the modular body loop is proven;
- complex animation requirements before body slots are designed;
- speculative systems not used by the descent/ascent loop.
