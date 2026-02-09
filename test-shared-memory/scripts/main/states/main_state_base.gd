@abstract
extends StateBase
##
class_name MainStateBase

var main : Main:
	set(value):
		controlled_node = value
	get:
		return controlled_node
