function boop()
  print( 'boop' )
  bluebear.engine.queue_callback( 270, beep )
end

function beep()
  print( 'beep' )
end

bluebear.engine.queue_callback( 180, boop )
