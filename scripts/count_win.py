#!/usr/bin/python

f = open('bwapi-data/logs/wins_loses.log', 'r')
wins = 0
loses = 0
for l in f:
    if 'won' in l:
        wins += 1
    else:
        loses += 1

print wins
print loses



