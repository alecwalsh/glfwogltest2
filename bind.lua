deltaTime = 0
lastTime = 0
RotSpeed = 0
reverse = false

lambdaCalled = false

function loop()
    deltaTime = cpp.elapsedTime - lastTime
    lastTime = cpp.elapsedTime
    
    if not reverse then
        RotSpeed = RotSpeed + deltaTime
    else
        RotSpeed = RotSpeed - deltaTime
    end
    
    if RotSpeed > 5 and not reverse then
        reverse = true
    end
    
    if RotSpeed <= 0.5 and reverse then
        reverse = false
    end
    
    cpp.RotSpeed = RotSpeed
    Tick()
    if not lambdaCalled then
        LambdaTest()
        lambdaCalled = true
    end
end
