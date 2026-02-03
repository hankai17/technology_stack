import torch
import torchvision
from torch.utils import data
from torchvision import transforms
from d2l import torch as d2l
import matplotlib.pyplot as plt

d2l.use_svg_display()

def get_dataloader_workers():  #@save
    return 4 														# 使用4个进程来读取数据

def get_fashion_mnist_labels(labels):  #@save
    """返回Fashion-MNIST数据集的文本标签"""
    text_labels = ['t-shirt', 'trouser', 'pullover', 'dress', 'coat', 'sandal', 'shirt', 'sneaker', 'bag', 'ankle boot']
    return [text_labels[int(i)] for i in labels]

def show_images(imgs, num_rows, num_cols, titles=None, scale=1.5):  #@save
    """绘制图像列表"""                                              #
    figsize = (num_cols * scale, num_rows * scale)                  # 整个画布大小 大概意思是将imgs诸多图片 映射到row*col的画布|表格中 axes是这个
                                                                    #   画布中的每个图片元素
    _, axes = d2l.plt.subplots(num_rows, num_cols, figsize=figsize) # 创建网格图
    axes = axes.flatten()                                           # 将其展平为一维数组
    for i, (ax, img) in enumerate(zip(axes, imgs)):
        if torch.is_tensor(img):
            ax.imshow(img.numpy())                                  # 图片张量
        else:
            ax.imshow(img)                                          # PIL图片
        ax.axes.get_xaxis().set_visible(False)
        ax.axes.get_yaxis().set_visible(False)
        if titles:
            ax.set_title(titles[i])
    return axes

def load_data_fashion_mnist(batch_size, resize=None):  #@save
    """下载Fashion-MNIST数据集，然后将其加载到内存中"""
    trans = [transforms.ToTensor()]                                 # 创建数据转换管道
                                                                    # 初始化一个转换列表，只包含 ToTensor() 转换
                                                                    # ToTensor()：将PIL图像或numpy数组转换为PyTorch张量，并归一化到[0,1]
    if resize:
        trans.insert(0, transforms.Resize(resize))
    trans = transforms.Compose(trans)
    mnist_train = torchvision.datasets.FashionMNIST(
        root="../data", train=True, transform=trans, download=True) # 应用上面定义的转换管道
    mnist_test = torchvision.datasets.FashionMNIST(
        root="../data", train=False, transform=trans, download=True)
    return (data.DataLoader(mnist_train, batch_size, shuffle=True,
                            num_workers=get_dataloader_workers()),
            data.DataLoader(mnist_test, batch_size, shuffle=False,
                            num_workers=get_dataloader_workers()))

# 通过ToTensor实例将图像数据从PIL类型变换成32位浮点数格式，
# 并除以255使得所有像素的数值均在0～1之间
trans = transforms.ToTensor()
mnist_train = torchvision.datasets.FashionMNIST(                    # 将Fashion-MNIST数据集下载并读取到内存中
    root="../data", train=True, transform=trans, download=True)
mnist_test = torchvision.datasets.FashionMNIST(
    root="../data", train=False, transform=trans, download=True)    # 测试数据集不会用于训练，只用于评估模型性能

#print(len(mnist_train), len(mnist_test))
#print(mnist_train[0][0].shape)                                      # 图像的高度和宽度均为28像素 通道数为1(黑白图片是1 彩色图片是3(rgb))

#X, y = next(iter(data.DataLoader(mnist_train, batch_size=18)))      # X: 图像张量，形状为 (18, 1, 28, 28) # y: 标签张量，形状为 (18,) 包含18个数字标签（0-9）
                                                                    # 例如: tensor([9, 0, 0, 3, 0, 2, 7, 2, 5, 5, ...])
#show_images(X.reshape(18, 28, 28), 2, 9, titles=get_fashion_mnist_labels(y));
#plt.show()

#batch_size = 256
#train_iter = data.DataLoader(mnist_train, batch_size, shuffle=True,
#                             num_workers=get_dataloader_workers())
#
#timer = d2l.Timer()
#for X, y in train_iter:
#    continue
#print(f'{timer.stop():.2f} sec')

train_iter, test_iter = load_data_fashion_mnist(32, resize=64)
for X, y in train_iter:
    print(X.shape, X.dtype, y.shape, y.dtype)
    break

