#@save
def show_heatmaps(matrices, xlabel, ylabel, titles=None, figsize=(2.5, 2.5),
                  cmap='Reds'):         # 4 维张量，形状 (num_rows, num_cols, h, w): 最外两层是子图网格的排布，内两层是每个子图里要画的热图矩阵
    """显示矩阵热图"""
    d2l.use_svg_display()
    num_rows, num_cols = matrices.shape[0], matrices.shape[1]           # 取出网格行列数
    fig, axes = d2l.plt.subplots(num_rows, num_cols, figsize=figsize,   # 创建子图网格
                                 sharex=True, sharey=True, squeeze=False)   # num_rows × num_cols：创建这么多子图。
    for i, (row_axes, row_matrices) in enumerate(zip(axes, matrices)):
        for j, (ax, matrix) in enumerate(zip(row_axes, row_matrices)):
            pcm = ax.imshow(matrix.detach().numpy(), cmap=cmap)
            if i == num_rows - 1:
                ax.set_xlabel(xlabel)
            if j == 0:
                ax.set_ylabel(ylabel)
            if titles:
                ax.set_title(titles[j])
    fig.colorbar(pcm, ax=axes, shrink=0.6);
