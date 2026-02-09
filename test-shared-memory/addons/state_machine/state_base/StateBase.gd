@abstract
extends Node
## State Base class for State Machine.
class_name StateBase

## referencia del nodo a controlar
@onready var controlled_node : Node = self.owner

## referencia de la máquina de estados
var state_machine : StateMachine = null

#region metodos comúnes

## Metodo que se ejecuta al entrar en el estado
func start() -> void:
	pass

## Metodo que se ejecuta al salir del estado
func end() -> void:
	pass

#endregion
