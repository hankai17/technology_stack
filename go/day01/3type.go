1bool
    var v1 bool
    v1 = true
    v2 := (1 == 2) //v2���Զ��Ƶ�Ϊbool����
    �������Ͳ�֧���������͵ĸ�ֵ����֧��ǿ������ת��
    eg :
    var b bool
    b = 1 //err
    b = bool(1) //err

2����
    int �� int32�����ֲ�ͬ���� �������������ת��
    eg :
    var v2 int32
    v1 := 64 //�Ƶ�Ϊint
    v2 = v1 //error
    ��֧��ǿת
    v2 = int32(v1)

    ���Ͳ�ͬ����ֱ�ӱȽ� int �� int32����ֱ�ӱȽ�

    ^2 Ϊʲô2ȡ������-3��

3������
    float64 float32
    var f1 float32
    f1 = 12
    f2 := 12.0 //�Ƶ���float64  �����Ƶ���int

    �������Ƚϣ���Ϊ����������ȷ
    import "math"
    func is_equal(f1, f2, p float64) bool {
        return math.Abs(f1 - f2) < p
    }

4��������
    var v1 complex64
    v1 = 3.2 + 12i
    v2 := 3.2 +12i
    v3 := complex(3.2, 12) //3����ͬ
    real(v3) ��ȡʵ��3.2 img(v3)��ȡ�鲿12

5�ַ���
    var str string
    string = "hello world"
    ch := str[0]
    fmt.printf("The length of \"%s\" is %d \n", str, len(str)) //�����ڵ����ŵ�ת��  11
    fmt.printf("The first character of \"%s\" is %c \n", str, ch) //�����ڵ����ŵ�ת��

    str := "mike dog"
    str[0] = 'c' //error

    �ַ���������
        str1 + str2 �ַ�������
        len(str1) �ַ�������
    �ַ���������
        �ֽ����鷽ʽ������
        str := "hello,�㻵"
        n := len(str)
        for i := 0; i < n; i++ {
            ch := str[i] //ÿ���ַ�����Ϊbyte
            fmt.Println(i,ch)
        } //���������� �� ��Ӧ��utf8 ��ֵ  һ�����������ֽڱ�ʾutf8ֵ
        unicode�ַ�������
        str := "hello,�㻵"
        for i, ch := range str {
            fmt.Println(i, ch)  //ÿ���ַ�����Ϊrune
        } //һ������һ��unicodeֵ
6�ַ����ͣ�
    go֧�������ַ����ͣ�
        byte(uint8�ı���) ����utf8�ַ��ĵ����ֽ�
        rune ����unicode�ַ�

7����:
    Ҳ��"Ԫ��"�ĸ���
    �����������
       [32]byte
       [2*N] struct { x, y int32 }
       [1000]*float64 //ָ������
       [3][5]int //��ά���� 3��Ԫ��
       [2][2][2]float64  �ȼ���[2]([2]([2]float64))

       ���鳣������
       array := [5]int{1, 2, 3, 4, 5}

       arr_len := len(arr) //�ó���Ԫ�ظ���
       Ԫ�ط���:
            for i := 0; i< len(array); i++ {
                fmt.Println("element", i, "of array is", array[i])
            }
            �ɼ�Ϊ
            for i, v := range array {
                fmt.Println("Array element [", i, "]=",v) //range����������ֵ
            }
       ֵ���ͣ�
            go������һ��ֵ���ͣ����ڸ�ֵ�ʹ���ʱ�ᷢ������ ����һ������ (û��c������ָ���ζ��)
            ������������Ƭ�ĸ���

func main() {
    //array [2][3]int 
    array := {{1,1,1},{1,1,1}}
    for i := 0; i< len(array); i++ {
        fmt.Println("element", i, "of array is", array[i])
    }
}

8������Ƭ��
    ��Ϊ���鲻��vector�������Զ�̬����
    �������鴴��slice ������
    ֱ�Ӵ�����
    make()����
    my_slice := make([]int, 5)  //Ԫ�ظ���Ϊ5 ��ʼֵΪ0
    my_slice := make([]int, 5, 10)  //Ԫ�ظ���Ϊ5 ��ʼֵΪ0 Ԥ��10���ռ� len(my_slice)��5 cap(my_slice)��10
    my_slice := []int{1, 2, 3, 4, 5}

    ��̬������
        �Կռ任ʱ�� ��Ԥ����һЩ�ռ�
        ������
        append(slice, 1, 2, 3) //��������Ӻ���
        append(slice, slice2...) //���һ��slice ע������...
    ����������Ƭ����������Ƭ��
        old_slice := []int{1, 2, 3, 4}
        new_slice := old_slice[:3]
    ���ݸ��ƣ�
        slice1 := []int{1, 2, 3, 4, 5}
        slice2 := []int{5, 4, 3}
        copy(slice2, slice1) //����1��ǰ3��Ԫ�� 
        copy(slice1, slice2)  //ͬ��

9 type�ؼ��ּ�������
    ����ṹ��
    ����������
        type name string //name��string��ͬ
    �ṹ�� ��Ƕ������Ա
        type person struct {
          string  //һ��string���͵�������Ա����
          age  int
        }

    ���庯������
        type handler func(name string) int //handler��һ����������/��������


