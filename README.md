Build linux:
mkdir build && cd build

nano config.json
then put into config json:
api_key	"Your API KEY"
region	"ru/eu/sea"
item_id	"y3nmw (find your lo at https://github.com/EXBO-Studio/stalcraft-database/tree/main/ru)"

cmake ..
make -j$(nproc)

Build windows:

mkdir build
cd build

nano config.json
then put into config json:
api_key	"Your API KEY"
region	"ru/eu/sea"
item_id	"y3nmw (find your lo at https://github.com/EXBO-Studio/stalcraft-database/tree/main/ru)"

cmake ..
cmake --build .
