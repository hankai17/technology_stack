 #apt-get install jupyter
 #wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
 #chmod +x Miniconda3-latest-Linux-x86_64.sh 
 #sh Miniconda3-latest-Linux-x86_64.sh -b
 #ll ~/miniconda3/
 #~/miniconda3/bin/conda init
 #conda config --set auto_activate_base false

#conda activate base
#conda deactivate

# 接受main频道的服务条款
conda tos accept --override-channels --channel https://repo.anaconda.com/pkgs/main
# 接受r频道的服务条款
conda tos accept --override-channels --channel https://repo.anaconda.com/pkgs/r

#conda create --name d2l python=3.9 -y
#conda activate d2l
