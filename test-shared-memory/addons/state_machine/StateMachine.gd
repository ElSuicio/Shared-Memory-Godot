extends Node
## State Machine class.
class_name StateMachine

## referencia del nodo a controlar
@onready var controlled_node : Node = self.owner

## estado por defecto
@export var default_state : StateBase = null

## estado en ejecución
var current_state : StateBase = null

## inicialización de variables y ejecución del metodo start
func _state_start() -> void:
	## configuración del estado
	current_state.controlled_node = controlled_node
	current_state.state_machine = self
	current_state.start()

func _state_default_start() -> void:
	current_state = default_state
	_state_start()

## metodo para cambiar de estado
func change_to(new_state : String) -> void:
	if(current_state and current_state.has_method("end")):
		current_state.end()
	current_state = get_node(new_state)
	_state_start()

func _ready() -> void:
	call_deferred("_state_default_start")

#region metodos en cada frame

func _process(delta: float) -> void:
	if(current_state and current_state.has_method("on_process")):
		current_state.on_process(delta)

func _physics_process(delta: float) -> void:
	if(current_state and current_state.has_method("on_physics_process")):
		current_state.on_physics_process(delta)

func _input(event: InputEvent) -> void:
	if(current_state and current_state.has_method("on_input")):
		current_state.on_input(event)

func _unhandled_input(event: InputEvent) -> void:
	if(current_state and current_state.has_method("on_unhandled_input")):
		current_state.on_unhandled_input(event)

func _unhandled_key_input(event: InputEvent) -> void:
	if(current_state and current_state.has_method("on_unhandled_key_input")):
		current_state.on_unhandled_key_input(event)

#endregion
