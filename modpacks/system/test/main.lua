print( ..., 'This is a modpack' )

local function register()
  --bluebear.gfx.register_models( { 'acme.floral.penis_shaped_flower', '/path/to/modpack/penis_shaped_flower.gltf' } )

  local WaterLevel = { water_level = 100, is_watering = false }

  function WaterLevel:get_interactions( player, lot )
    if is_watering then
      return nil
    end

    return {
      {
        name = 'Water',
        criteria = bluebear.util.bind( self.show_water_interaction, self ),
        action = bluebear.util.bind( self.begin_water, self, player )
      }
    }
  end

  function WaterLevel:show_water_interaction()
    return water_level < 70
  end

  function WaterLevel:begin_water( player )
    self.is_watering = true

    local interaction_queue = player:get_component( 'system.component.interaction_queue' )
    interaction_queue:enqueue( {
      name = 'Water',
      background_color = bluebear.util.types.Vec4.new( 255 / self.water_level, 0, 0, 255 ),
      on_approach = bluebear.util.bind( self.bring_to, self, player, interaction_queue ),
      on_interact = bluebear.util.bind( self.water, self, player, interaction_queue ),
      on_conclude = bluebear.util.bind( self.wrap_up, self, player, interaction_queue ),
      on_abort = bluebear.util.bind( self.wrap_up, self, player, interaction_queue )
    } )
  end

  function WaterLevel:bring_to( player, interaction_queue )
    player:get_component( 'system.component.navigator' ):find_path_to( self ):then_call( function()
      interaction_queue:change_state( 'interact' )
    end )
  end

  function WaterLevel:water( player, interaction_queue )
    player:get_component( 'system.component.animator' ):play_animation( 'watering_pot' ):then_call( function()
      self.water_level = 100
      interaction_queue:change_state( 'conclude' )
    end )
  end

  function WaterLevel:wrap_up( player, interaction_queue )
    player:get_component( 'system.component.animator' ):play_animation( 'watering_pot_put_away' ):then_call( function()
      interaction_queue:finished()
    end )
  end

  function WaterLevel:on_place( lot )
    print( 'system.component.waterlevel', 'Water level initialised after entity created' )
  end

  bluebear.entity.register_component( 'system.component.waterlevel', WaterLevel )
  bluebear.entity.register_entity( 'acme.floral.exotic_plant', { 'system.component.model_manager', 'system.component.plant_waterlevel' } )

  -- example usage: create_new_entity will place an entity instance in a C++ vector and call on_place on each component
  bluebear.entity.create_new_entity( 'acme.floral.exotic_plant', {
    --'system.component.model_manager' = {
    --  default_models = { 'acme.floral.penis_shaped_flower' }
    --}
  } )
end
