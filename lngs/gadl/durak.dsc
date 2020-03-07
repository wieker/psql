
declare
		unit card { int suit; int value; }
		side { card pack[]; }
		map { card pack[]; card bottom; card desk[2][6]; card trash[]; }
		alias bottom desk[1];
		alias top desk[2];
		
defun moveround():
		i = 0;
		while (pack.size > 0 and i < 6) {
			j = 0;
			while (pack.size > 0 and j < players.size) {
				if (players[j].pack < 6)
					move(pack, players[j].pack);
				j ++;
			}
		}

defprec init:
		pack = (select t1.number as suit, t2.number as value from intlist(9) t1, intlist(2) t2);
		pack.shuffle;
		ref = pack.last;
		move_round();

defprec target:
		side.pack.empty and desk.empty and pack.empty => true
		false

defrule move(side, card, hex):
        not hex on desk => false
        continue
defrule move(side, card, hex):
        side == sides.last => continue
        hex on top => false
        desk.count = sides.last.pack.count => false
        desk.empty => true
        card.value in ( select card.value from desk ) => true
        false
defrule move(side, card, hex):
        not player == sides.last => continue
        hex on bottom => false
        card.value > hex.down.card.value and card.suit == hex.down.card.suit => true
        card.suit == kozyr_suit and not hex.down.card.suit == kozyr_suit => true
        false

defhook end(round):
        if (round.dirty == 1 and bottom.count < 6 and sides.last.pack.count > 0) continue;
        c = (select card from bottom where card.hex.top.card == null);
        if (c.count > 0) {
			move(desk, side.pack);
			move_round(pack, players.pack);
			sides.rotate(2);
		} else {
			move(desk, trash);
			move_round(pack, players.pack, 6);
			sides.rotate(1);
		}
