import bpy
import math

rot_x_file = open("/home/shuto/Downloads/rotate_x.txt", 'w')
rot_z_file = open("/home/shuto/Downloads/rotate_z.txt", 'w')
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
    for frames in range(scene.frame_start, scene.frame_end+1):
        scene.frame_set(frames) 
        conv_x_angle = round(reMap(math.degrees(rot.x), 9, -10, 145, 80))
        conv_z_angle = round(reMap(math.degrees(rot.z), 4, -4, 85, 120))
        msc = frame_to_msc(frames)
        current_msc = msc - last_msc
        last_msc = msc
#        rot_x_file.write(
#        '# include <AnimationServe.h>'+
#        '\n'+'\n'+
#        'constexpr int dogma_pitch_len = '+str(scence.frame_end+1)+';'+
#        'KeyFrame dogma_pitch_frames[dogma_pitch_len] = {'+
#        '{'+str(conv_x_angle)+','+str(current_msc)+'},\n')
        
        rot_z_file.write(
        '# include <AnimationServe.h>\n\n'+
        'constexpr int dogma_roll_len = ' +str(scence.frame_end+1)+';'+
        'KeyFrame dogma_roll_frames[dogma_roll_len] = {'+
        '{'+str(conv_z_angle)+','+str(current_msc)+'},\n')
        # rot_z_file.write('{'+str(conv_z_angle)+','+str(current_msc)+'},\n')
 
    rot_x_file.close()
    rot_z_file.close()
    
write_rotation_files()
