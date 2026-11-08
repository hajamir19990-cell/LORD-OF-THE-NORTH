extends Node3D
## First playable foundation for LORD OF THE NORTH.
## World geometry is generated procedurally to avoid fragile runtime mesh buffers.

const WORLD_LIMIT := 420.0
const WALK_SPEED := 7.0
const RUN_SPEED := 11.0
const SITES := [
	{"name":"WINTERFELL", "house":"House Stark", "p":Vector3(0, 0, 0), "scale":1.0},
	{"name":"THE DREADFORT", "house":"House Bolton", "p":Vector3(85, 0, -95), "scale":0.8},
	{"name":"WHITE HARBOR", "house":"House Manderly", "p":Vector3(-150, 0, -115), "scale":1.0},
	{"name":"BEAR ISLAND", "house":"House Mormont", "p":Vector3(230, 0, -45), "scale":0.7},
	{"name":"KARHOLD", "house":"House Karstark", "p":Vector3(-210, 0, -245), "scale":0.7},
	{"name":"LAST HEARTH", "house":"House Umber", "p":Vector3(15, 0, -265), "scale":0.7},
	{"name":"DEEPWOOD MOTTE", "house":"House Glover", "p":Vector3(205, 0, -260), "scale":0.8},
	{"name":"GREYWATER WATCH", "house":"House Reed", "p":Vector3(-100, 0, -365), "scale":0.7},
	{"name":"THE WALL", "house":"The Night's Watch", "p":Vector3(45, 0, -430), "scale":1.1}
]
var player: CharacterBody3D
var camera_pivot: Node3D
var spring_arm: SpringArm3D
var camera: Camera3D
var sun: DirectionalLight3D
var move_input := Vector2.ZERO
var look_touch_id := -1
var last_look := Vector2.ZERO
var menu_open := true
var current_quest := "The Wolf's Oath — Find Lord Eddard Stark in Winterfell."
var gold := 100
var renown := 0
var stamina := 100.0
var day_clock := 0.28
var dialog_panel: PanelContainer
var dialog_label: Label
var hud_quest: Label
var hud_location: Label
var hud_stats: Label
var last_site := "WINTERFELL"

func _ready() -> void:
	_build_environment()
	_build_world()
	_build_player()
	_build_hud()
	_build_menu()
	_load_game()
	_update_hud()

func _process(delta: float) -> void:
	day_clock = fposmod(day_clock + delta * 0.0025, 1.0)
	if sun:
		sun.rotation_degrees.x = -28.0 - sin(day_clock * TAU) * 18.0
		sun.light_energy = 0.75 + maxf(0.0, sin(day_clock * TAU)) * 0.65
	if not menu_open:
		_update_player(delta)
		_update_location()

func _build_environment() -> void:
	var world_env := WorldEnvironment.new()
	var env := Environment.new()
	env.background_mode = Environment.BG_SKY
	var sky := Sky.new()
	var sky_mat := ProceduralSkyMaterial.new()
	sky_mat.sky_top_color = Color(0.10, 0.16, 0.25)
	sky_mat.sky_horizon_color = Color(0.62, 0.68, 0.72)
	sky_mat.ground_bottom_color = Color(0.08, 0.11, 0.14)
	sky_mat.ground_horizon_color = Color(0.45, 0.51, 0.55)
	sky.sky_material = sky_mat
	env.sky = sky
	env.ambient_light_source = Environment.AMBIENT_SOURCE_COLOR
	env.ambient_light_color = Color(0.56, 0.64, 0.72)
	env.ambient_light_energy = 0.65
	env.tonemap_mode = Environment.TONE_MAPPER_FILMIC
	env.fog_enabled = true
	env.fog_light_color = Color(0.58, 0.66, 0.72)
	env.fog_density = 0.0018
	world_env.environment = env
	add_child(world_env)
	sun = DirectionalLight3D.new()
	sun.rotation_degrees = Vector3(-42, -28, 0)
	sun.light_color = Color(0.82, 0.88, 1.0)
	sun.light_energy = 1.1
	sun.shadow_enabled = true
	add_child(sun)

func _build_world() -> void:
	var ground := MeshInstance3D.new()
	var plane := PlaneMesh.new()
	plane.size = Vector2(1000, 1000)
	ground.mesh = plane
	var snow := StandardMaterial3D.new()
	snow.albedo_color = Color(0.72, 0.80, 0.84)
	snow.roughness = 0.94
	snow.metallic = 0.02
	ground.material_override = snow
	ground.position.y = -0.15
	add_child(ground)
	_add_box(Vector3(1000, 0.4, 12), Vector3(0, -0.03, -165), _material(Color(0.43, 0.58, 0.65)), "Frozen river")
	_build_forest()
	for site in SITES:
		_build_settlement(site)
	_build_roads()
	_build_winterfell_npc()
	_build_wall()

func _build_forest() -> void:
	var rng := RandomNumberGenerator.new()
	rng.seed = 814201
	var trunk_mat := _material(Color(0.20, 0.15, 0.12))
	var pine_mat := _material(Color(0.10, 0.20, 0.19))
	var snow_mat := _material(Color(0.70, 0.79, 0.82))
	for i in range(150):
		var x := rng.randf_range(-390, 390)
		var z := rng.randf_range(-410, 320)
		if Vector2(x, z).length() < 42 or absf(z + 165) < 16:
			continue
		var h := rng.randf_range(3.0, 5.5)
		_add_cylinder(0.18, 0.25, h, Vector3(x, h * 0.5 - 0.1, z), trunk_mat)
		for layer in range(3):
			var cone_h := h * 0.55
			var cone := CylinderMesh.new()
			cone.top_radius = 0.0
			cone.bottom_radius = h * (0.28 - layer * 0.045)
			cone.height = cone_h
			var tree := MeshInstance3D.new()
			tree.mesh = cone
			tree.material_override = pine_mat if layer < 2 else snow_mat
			tree.position = Vector3(x, h * 0.38 + layer * h * 0.20, z)
			add_child(tree)

func _build_settlement(site: Dictionary) -> void:
	var root := Node3D.new()
	root.name = str(site.name).replace(" ", "_")
	root.position = site.p
	add_child(root)
	var s: float = site.scale
	var stone := _material(Color(0.34, 0.39, 0.42))
	var roof := _material(Color(0.20, 0.25, 0.29))
	_add_box(Vector3(18, 13, 18) * s, Vector3(0, 6.5 * s, 0), stone, "Keep", root)
	_add_box(Vector3(20, 2, 20) * s, Vector3(0, 13.2 * s, 0), roof, "Keep roof", root)
	for i in range(4):
		var a := float(i) * PI * 0.5
		var pos := Vector3(cos(a) * 12 * s, 4.5 * s, sin(a) * 12 * s)
		_add_box(Vector3(3, 9, 3) * s, pos, stone, "Tower", root)
	for i in range(5):
		var x := (float(i) - 2.0) * 7.0 * s
		var z := 22.0 * s + (i % 2) * 5.0 * s
		_add_box(Vector3(5, 4, 5) * s, Vector3(x, 2 * s, z), stone, "House", root)
		_add_box(Vector3(5.5, 1.1, 5.5) * s, Vector3(x, 4.4 * s, z), roof, "Roof", root)
	var sign := Label3D.new()
	sign.text = str(site.name)
	sign.font_size = 44
	sign.pixel_size = 0.008
	sign.position = Vector3(0, 19 * s, 0)
	sign.modulate = Color(0.92, 0.86, 0.68)
	root.add_child(sign)

func _build_roads() -> void:
	var road_mat := _material(Color(0.47, 0.52, 0.54))
	for i in range(SITES.size() - 1):
		var a: Vector3 = SITES[i].p
		var b: Vector3 = SITES[i + 1].p
		var mid := (a + b) * 0.5
		var road := _add_box(Vector3(3.5, 0.08, a.distance_to(b)), Vector3(mid.x, 0.02, mid.z), road_mat, "Northern road")
		road.look_at(Vector3(b.x, 0.02, b.z), Vector3.UP)

func _build_winterfell_npc() -> void:
	var npc := Node3D.new()
	npc.name = "Eddard Stark"
	npc.position = Vector3(0, 0, 13)
	add_child(npc)
	_add_cylinder(0.42, 0.42, 1.8, Vector3(0, 0.9, 0), _material(Color(0.22, 0.28, 0.32)), "NPC body", npc)
	_add_sphere(0.32, Vector3(0, 2.0, 0), _material(Color(0.72, 0.58, 0.48)), "NPC head", npc)
	var label := Label3D.new()
	label.text = "EDDARD STARK"
	label.font_size = 34
	label.position = Vector3(0, 2.8, 0)
	label.modulate = Color(0.94, 0.83, 0.59)
	npc.add_child(label)

func _build_wall() -> void:
	var wall_mat := _material(Color(0.64, 0.73, 0.77))
	for i in range(14):
		_add_box(Vector3(12, 44, 12), Vector3(-35 + i * 12, 22, -438), wall_mat, "THE WALL")
	_add_box(Vector3(180, 7, 20), Vector3(43, 44, -438), wall_mat, "Wall crown")

func _build_player() -> void:
	player = CharacterBody3D.new()
	player.name = "Player"
	var collision := CollisionShape3D.new()
	var capsule := CapsuleShape3D.new()
	capsule.radius = 0.48
	capsule.height = 1.9
	collision.shape = capsule
	collision.position.y = 0.95
	player.add_child(collision)
	var body := MeshInstance3D.new()
	var body_mesh := CapsuleMesh.new()
	body_mesh.radius = 0.46
	body_mesh.height = 1.85
	body.mesh = body_mesh
	body.material_override = _material(Color(0.18, 0.23, 0.28))
	body.position.y = 0.95
	player.add_child(body)
	var head := MeshInstance3D.new()
	var head_mesh := SphereMesh.new()
	head_mesh.radius = 0.27
	head_mesh.height = 0.54
	head.mesh = head_mesh
	head.material_override = _material(Color(0.72, 0.57, 0.46))
	head.position.y = 2.05
	player.add_child(head)
	add_child(player)
	player.position = Vector3(0, 0, 27)
	camera_pivot = Node3D.new()
	camera_pivot.position = Vector3(0, 1.55, 0)
	player.add_child(camera_pivot)
	spring_arm = SpringArm3D.new()
	spring_arm.spring_length = 7.0
	spring_arm.margin = 0.25
	camera_pivot.add_child(spring_arm)
	camera = Camera3D.new()
	camera.fov = 70
	spring_arm.add_child(camera)
	camera.current = true

func _build_hud() -> void:
	var layer := CanvasLayer.new()
	layer.name = "HUD"
	add_child(layer)
	var root := Control.new()
	root.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	root.mouse_filter = Control.MOUSE_FILTER_PASS
	layer.add_child(root)
	var top := _panel(root, Vector2(18, 18), Vector2(390, 116), Color(0.025, 0.055, 0.075, 0.88))
	var title := _label(top, "LORD OF THE NORTH  •  THE NORTH REMEMBERS", 17, Color(0.88, 0.77, 0.52))
	title.position = Vector2(14, 10)
	hud_stats = _label(top, "VITALITY  100     STAMINA  100     RENOWN  0", 13, Color(0.82, 0.87, 0.89))
	hud_stats.position = Vector2(14, 43)
	hud_location = _label(top, "LOCATION  •  THE NORTH", 13, Color(0.72, 0.82, 0.86))
	hud_location.position = Vector2(14, 69)
	var quest_box := _panel(root, Vector2.ZERO, Vector2(400, 94), Color(0.025, 0.055, 0.075, 0.88))
	quest_box.anchor_left = 1
	quest_box.anchor_right = 1
	quest_box.offset_left = -418
	quest_box.offset_right = -18
	quest_box.offset_top = 18
	quest_box.offset_bottom = 112
	hud_quest = _label(quest_box, "THE WOLF'S OATH\nFind Lord Eddard Stark in Winterfell.", 14, Color(0.91, 0.88, 0.78))
	hud_quest.position = Vector2(14, 12)
	var bottom := _label(root, "DRAG RIGHT SIDE TO LOOK  •  EXPLORE THE NORTH", 12, Color(0.78, 0.84, 0.86))
	bottom.anchor_left = 0.5
	bottom.anchor_right = 0.5
	bottom.anchor_top = 1
	bottom.anchor_bottom = 1
	bottom.position = Vector2(-165, -32)
	_add_touch_buttons(root)
	dialog_panel = _panel(root, Vector2.ZERO, Vector2(520, 130), Color(0.025, 0.055, 0.075, 0.96))
	dialog_panel.anchor_left = 0.5
	dialog_panel.anchor_right = 0.5
	dialog_panel.anchor_top = 1
	dialog_panel.anchor_bottom = 1
	dialog_panel.offset_left = -260
	dialog_panel.offset_right = 260
	dialog_panel.offset_top = -155
	dialog_panel.offset_bottom = -25
	dialog_panel.visible = false
	dialog_label = _label(dialog_panel, "", 16, Color(0.94, 0.91, 0.82))
	dialog_label.position = Vector2(18, 18)
	dialog_label.size = Vector2(484, 92)
	dialog_label.autowrap_mode = TextServer.AUTOWRAP_WORD

func _add_touch_buttons(root: Control) -> void:
	var controls := [
		{"text":"◀", "p":Vector2(24, -116), "v":Vector2(-1, 0)},
		{"text":"▶", "p":Vector2(150, -116), "v":Vector2(1, 0)},
		{"text":"▲", "p":Vector2(87, -176), "v":Vector2(0, -1)},
		{"text":"▼", "p":Vector2(87, -56), "v":Vector2(0, 1)}
	]
	for item in controls:
		var button := Button.new()
		button.text = item.text
		button.position = item.p
		button.position.y += get_viewport().get_visible_rect().size.y
		button.size = Vector2(54, 54)
		button.modulate = Color(0.82, 0.78, 0.65, 0.88)
		button.button_down.connect(func(): move_input = item.v)
		button.button_up.connect(func(): move_input = Vector2.ZERO)
		root.add_child(button)
	var interact := Button.new()
	interact.text = "INTERACT"
	interact.anchor_left = 1
	interact.anchor_right = 1
	interact.anchor_top = 1
	interact.anchor_bottom = 1
	interact.offset_left = -150
	interact.offset_right = -24
	interact.offset_top = -112
	interact.offset_bottom = -34
	interact.pressed.connect(_interact)
	root.add_child(interact)

func _build_menu() -> void:
	var layer := CanvasLayer.new()
	layer.name = "MainMenu"
	add_child(layer)
	var shade := ColorRect.new()
	shade.color = Color(0.015, 0.03, 0.045, 0.88)
	shade.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	layer.add_child(shade)
	var panel := PanelContainer.new()
	panel.custom_minimum_size = Vector2(440, 0)
	panel.set_anchors_and_offsets_preset(Control.PRESET_CENTER)
	panel.position = Vector2(-220, -250)
	layer.add_child(panel)
	var list := VBoxContainer.new()
	list.add_theme_constant_override("separation", 12)
	panel.add_child(list)
	var logo := _label(list, "❄  LORD OF THE NORTH  ❄", 28, Color(0.88, 0.76, 0.49))
	logo.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	var subtitle := _label(list, "THE NORTH REMEMBERS", 13, Color(0.68, 0.77, 0.81))
	subtitle.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	_menu_button(list, "NEW GAME", func(): _new_game(layer))
	_menu_button(list, "CONTINUE", func(): _continue_game(layer))
	_menu_button(list, "SETTINGS", func(): _show_message("SETTINGS", "Graphics: Mobile 3D\nCamera sensitivity and audio controls will be expanded in the next milestone."))
	_menu_button(list, "HOW TO PLAY", func(): _show_message("HOW TO PLAY", "Explore the North, meet its people, discover settlements, and complete quests. Use the left controls to move and drag the right side to look around."))
	_menu_button(list, "DEVELOPERS", func(): _show_message("DEVELOPERS", "AmirReza — Game Director & Creator\nRaha — Creative & Technical Assistant"))
	_menu_button(list, "EXIT", func(): get_tree().quit())
	var foot := _label(list, "A NORTHERN ADVENTURE  •  BUILD 0.1.0", 11, Color(0.55, 0.64, 0.69))
	foot.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	menu_open = true

func _menu_button(parent: VBoxContainer, text_value: String, action: Callable) -> void:
	var button := Button.new()
	button.text = text_value
	button.custom_minimum_size = Vector2(380, 48)
	button.pressed.connect(action)
	parent.add_child(button)

func _panel(parent: Control, pos: Vector2, size: Vector2, color: Color) -> PanelContainer:
	var panel := PanelContainer.new()
	panel.position = pos
	panel.size = size
	var style := StyleBoxFlat.new()
	style.bg_color = color
	style.corner_radius_top_left = 14
	style.corner_radius_top_right = 14
	style.corner_radius_bottom_left = 14
	style.corner_radius_bottom_right = 14
	panel.add_theme_stylebox_override("panel", style)
	parent.add_child(panel)
	return panel

func _label(parent: Control, text_value: String, font_size: int, color: Color) -> Label:
	var label := Label.new()
	label.text = text_value
	label.add_theme_font_size_override("font_size", font_size)
	label.add_theme_color_override("font_color", color)
	parent.add_child(label)
	return label

func _material(color: Color) -> StandardMaterial3D:
	var mat := StandardMaterial3D.new()
	mat.albedo_color = color
	mat.roughness = 0.9
	return mat

func _add_box(size: Vector3, pos: Vector3, mat: Material, node_name := "Box", parent: Node = self) -> MeshInstance3D:
	var node := MeshInstance3D.new()
	var mesh := BoxMesh.new()
	mesh.size = size
	node.mesh = mesh
	node.material_override = mat
	node.position = pos
	node.name = node_name
	parent.add_child(node)
	return node

func _add_cylinder(top: float, bottom: float, height: float, pos: Vector3, mat: Material, node_name := "Cylinder", parent: Node = self) -> MeshInstance3D:
	var node := MeshInstance3D.new()
	var mesh := CylinderMesh.new()
	mesh.top_radius = top
	mesh.bottom_radius = bottom
	mesh.height = height
	node.mesh = mesh
	node.material_override = mat
	node.position = pos
	node.name = node_name
	parent.add_child(node)
	return node

func _add_sphere(radius: float, pos: Vector3, mat: Material, node_name := "Sphere", parent: Node = self) -> MeshInstance3D:
	var node := MeshInstance3D.new()
	var mesh := SphereMesh.new()
	mesh.radius = radius
	mesh.height = radius * 2
	node.mesh = mesh
	node.material_override = mat
	node.position = pos
	node.name = node_name
	parent.add_child(node)
	return node

func _update_player(delta: float) -> void:
	var axis := move_input
	if Input.is_key_pressed(KEY_A): axis.x -= 1
	if Input.is_key_pressed(KEY_D): axis.x += 1
	if Input.is_key_pressed(KEY_W): axis.y -= 1
	if Input.is_key_pressed(KEY_S): axis.y += 1
	axis = axis.limit_length(1.0)
	var forward := -camera_pivot.global_basis.z
	var right := camera_pivot.global_basis.x
	var direction := right * axis.x + forward * -axis.y
	direction.y = 0
	direction = direction.normalized()
	var speed := RUN_SPEED if Input.is_key_pressed(KEY_SHIFT) else WALK_SPEED
	if direction.length() > 0.01:
		player.velocity.x = direction.x * speed
		player.velocity.z = direction.z * speed
		player.rotation.y = atan2(direction.x, direction.z)
	else:
		player.velocity.x = move_toward(player.velocity.x, 0.0, WALK_SPEED * delta * 4)
		player.velocity.z = move_toward(player.velocity.z, 0.0, WALK_SPEED * delta * 4)
	player.velocity.y -= 22.0 * delta
	player.move_and_slide()
	player.position.x = clampf(player.position.x, -WORLD_LIMIT, WORLD_LIMIT)
	player.position.z = clampf(player.position.z, -WORLD_LIMIT - 40, WORLD_LIMIT)
	stamina = clampf(stamina + (10.0 if axis.length() < 0.1 else -6.0) * delta, 0.0, 100.0)

func _unhandled_input(event: InputEvent) -> void:
	if menu_open:
		return
	if event is InputEventScreenTouch:
		if event.pressed and event.position.x > get_viewport().get_visible_rect().size.x * 0.42:
			look_touch_id = event.index
			last_look = event.position
		elif not event.pressed and event.index == look_touch_id:
			look_touch_id = -1
	if event is InputEventScreenDrag and event.index == look_touch_id:
		var delta := event.position - last_look
		last_look = event.position
		_rotate_camera(delta)
	if event is InputEventMouseMotion and Input.is_mouse_button_pressed(MOUSE_BUTTON_RIGHT):
		_rotate_camera(event.relative)

func _rotate_camera(delta: Vector2) -> void:
	camera_pivot.rotation.y -= delta.x * 0.004
	spring_arm.rotation.x = clampf(spring_arm.rotation.x - delta.y * 0.003, -0.85, 0.25)

func _update_location() -> void:
	var nearest := "THE NORTH"
	var best := INF
	for site in SITES:
		var d: float = player.global_position.distance_to(site.p)
		if d < best:
			best = d
			nearest = site.name
	if nearest != last_site:
		last_site = nearest
		hud_location.text = "LOCATION  •  " + nearest
		if best < 35:
			current_quest = "Explore " + nearest + " and speak with its people."
			_update_hud()

func _interact() -> void:
	if player.global_position.distance_to(Vector3(0, 0, 13)) < 8:
		dialog_label.text = "EDDARD STARK\nThe North is held by oaths, old roads, and the people who keep both. Your journey begins here."
		dialog_panel.visible = true
		renown += 5
		current_quest = "Visit the northern settlements and earn the trust of their lords."
		_save_game()
		_update_hud()
	else:
		_show_message("THE NORTH", "There is no one close enough to speak with. Explore the settlement and look around.")

func _show_message(title_text: String, body_text: String) -> void:
	var popup := AcceptDialog.new()
	popup.title = title_text
	popup.dialog_text = body_text
	add_child(popup)
	popup.popup_centered(Vector2i(520, 280))

func _new_game(layer: CanvasLayer) -> void:
	player.position = Vector3(0, 0, 27)
	player.velocity = Vector3.ZERO
	gold = 100
	renown = 0
	stamina = 100
	current_quest = "The Wolf's Oath — Find Lord Eddard Stark in Winterfell."
	dialog_panel.visible = false
	_save_game()
	_start_game(layer)

func _continue_game(layer: CanvasLayer) -> void:
	_load_game()
	_start_game(layer)

func _start_game(layer: CanvasLayer) -> void:
	menu_open = false
	layer.queue_free()

func _save_game() -> void:
	var cfg := ConfigFile.new()
	cfg.set_value("player", "position", player.position)
	cfg.set_value("player", "gold", gold)
	cfg.set_value("player", "renown", renown)
	cfg.set_value("player", "stamina", stamina)
	cfg.set_value("quest", "current", current_quest)
	cfg.save("user://north_save.cfg")

func _load_game() -> void:
	var cfg := ConfigFile.new()
	if cfg.load("user://north_save.cfg") == OK:
		player.position = cfg.get_value("player", "position", player.position)
		gold = int(cfg.get_value("player", "gold", 100))
		renown = int(cfg.get_value("player", "renown", 0))
		stamina = float(cfg.get_value("player", "stamina", 100.0))
		current_quest = str(cfg.get_value("quest", "current", current_quest))

func _update_hud() -> void:
	if hud_quest:
		hud_quest.text = "THE WOLF'S OATH\n" + current_quest
	if hud_stats:
		hud_stats.text = "VITALITY  100     STAMINA  %d     GOLD  %d     RENOWN  %d" % [roundi(stamina), gold, renown]
