from PIL import Image, ImageDraw
import random

def create_block(draw, index, color):
    # Рассчитываем позицию блока в сетке 16x16
    x_offset = (index % 16) * 16
    y_offset = (index // 16) * 16

    for x in range(16):
        for y in range(16):
            # Добавляем шум
            noise = random.randint(-15, 15)
            r = max(0, min(255, color[0] + noise))
            g = max(0, min(255, color[1] + noise))
            b = max(0, min(255, color[2] + noise))
            draw.point((x_offset + x, y_offset + y), fill=(r, g, b))

# Создаем полотно 256x256
atlas = Image.new('RGB', (256, 256), color=(40, 40, 40)) # Темно-серый фон
draw = ImageDraw.Draw(atlas)

# Словарь блоков: ID -> RGB цвет
blocks = {
    0:  (34, 139, 34),   # Grass (Green)
    1:  (139, 69, 19),   # Dirt (Brown)
    2:  (120, 120, 120), # Stone (Gray)
    3:  (210, 180, 140), # Sand (Tan)
    4:  (255, 250, 250), # Snow (White)
    5:  (70, 130, 180),  # Water/Ice (Steel Blue)
    6:  (101, 67, 33),   # Wood Log (Dark Brown)
    7:  (34, 100, 34),   # Leaves (Dark Green)
    8:  (178, 34, 34),   # Red Brick
    9:  (105, 105, 105), # Cobblestone
    10: (218, 165, 32),  # Gold Block
    11: (230, 230, 250), # Iron Block
    12: (0, 128, 128),   # Magic/Teal Block
    13: (75, 0, 130),    # Obsidian/Purple
    14: (20, 20, 20),    # Coal/Bedrock
    15: (255, 20, 147),  # Error/Missing Texture (Pink)
}

for block_id, color in blocks.items():
    create_block(draw, block_id, color)

atlas.save('terrain.png')
# print("Atlas with 16 blocks generated successfully!")
