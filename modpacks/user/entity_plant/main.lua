bluebear.engine.require_modpack( 'component_water' )

bluebear.entity.register_entity( 'game.entity.plant',
  {
    'system.component.model_manager',
    'game.component.plant.water_level'
  }
)
