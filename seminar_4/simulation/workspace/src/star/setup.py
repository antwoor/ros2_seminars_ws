from setuptools import find_packages, setup
import os
from glob import glob

package_name = 'star'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/' + package_name, ['package.xml']),
        # Эта строка добавляет файлы запуска
        (os.path.join('share', package_name, 'launch'), glob('launch/*.launch.py')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='root',
    maintainer_email='root@todo.todo',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
entry_points={
    'console_scripts': [
        'qr_decoder = star.qr_decoder:main',  # Если есть скрипт qr_decoder.py с функцией main
        'navigation = star.navigation:main',  # Если есть скрипт navigation.py с функцией main
    ],
},

)
