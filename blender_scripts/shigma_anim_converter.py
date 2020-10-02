import bpy
import math

rot_x_file = open("/home/shuto/makaizou/super_tompy/animation_texts/shigma_pitch.txt", 'w')
rot_z_file = open("/home/shuto/makaizou/super_tompy/animation_texts/shigma_roll.txt", 'w')
scene = bpy.context.scene
rot = bpy.context.object.pose.bones["Head"].rotation_quaternion

def reMap(value, maxInput, minInput, maxOutput, minOutput):
    value = maxInput if value > maxInput else value
    value = minInput if value < minInput else value

    inputSpan = maxInput - minInput
    outputSpan = maxOutput - minOutput

    scaledThrust = float(value - minInput) / float(inputSpan)

    return minOutput + (scaledThrust * outputSpan)

def frame_to_msc(frame):
    return round(frame/24 * 1000)
    

def write_rotation_files():
    last_msc = 0
    current_msc = 0
    rot_x_file.write(
        '#include <AnimationServo.h>\n\n'+
        'constexpr int shigma_pitch_len = ' +str(scene.frame_end+1)+';\n'+
        'KeyFrame shigma_pitch_frames[shigma_pitch_len] = {\n')
    rot_z_file.write(
        '#include <AnimationServo.h>\n\n'+
        'constexpr int shigma_roll_len = ' +str(scene.frame_end+1)+';\n'+
        'KeyFrame shigma_roll_frames[shigma_roll_len] = {\n')
    for frames in range(scene.frame_start, scene.frame_end+1):
        scene.frame_set(frames) 
        conv_x_angle = round(reMap(math.degrees(rot.x), 9, -10, 145, 80))
        conv_z_angle = round(reMap(math.degrees(rot.z), 4, -4, 85, 120))
        msc = frame_to_msc(frames)
        current_msc = msc - last_msc
        last_msc = msc
        rot_x_file.write('{'+str(conv_x_angle)+','+str(current_msc)+'},\n')
        rot_z_file.write('{'+str(conv_z_angle)+','+str(current_msc)+'},\n')
   
    rot_x_file.write('};\n')     
    rot_z_file.write('};\n')
    rot_x_file.close()
    rot_z_file.close()
    
write_rotation_files()
