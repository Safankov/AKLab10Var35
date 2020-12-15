#include <stdint.h>
#include <stdio.h>

int main()
{
    bool is_8086_88 = false;
    bool is_cpuid_supported = false;
    int32_t cpuid_max = 0;
    char cpuid_string[13] = { 0 };
    int32_t cpuid_signature = 0;
    int32_t cpuid_ecx_flags = 0;
    int32_t cpuid_edx_flags = 0;

    __asm
    {
        ; 1. ��������� ������� ������, ���������� ��������� � ������� 32-���������� ���������� � �������.
        pushf ; ��������� ����� �������� ������
        pop bx ; ������������ � BX
        mov ax, 0x0FFF ; �������� ���� 12-15 � AX
        and ax, bx
        push ax ; ��������� ����� �������� �������� ������
        popf
        pushf ; ���������� ������� ������
        pop ax ; ��������� ����� ������ �������� ������ � AX

        mov cx, 0xF000
        and ax, cx
        cmp ax, cx ; ���� ���� 12-15 �����������, ��� ��������� 8086/88
        jne non_8086 ; �������, ���� �� 8086/88

        cputype_8086:
        mov [is_8086_88], 1 ; ��� 8086/88 ���������
        jmp end

        non_8086:
        mov [is_8086_88], 0 ; ��� �� 8086/88 ���������

        ; 2. ��������� � ��������� ������� CPUID (����������� ���� ID �������� EFLAGS) � ���������� ������������ �������� ��������� �� ������.
        pushfd
        pop eax
        mov ebx, eax
        xor eax, 0x200000
        push eax
        popfd
        pushfd
        pop eax
        cmp eax, ebx
        jne cpuid_supported_label

        cpuid_not_supported_label:
        mov [is_cpuid_supported], 0
        jmp end

        cpuid_supported_label:
        mov [is_cpuid_supported], 1
        mov eax, 0
        cpuid
        mov [cpuid_max], eax

        ; 3. �������� ������ ������������� ������������� ���������� � ��������� �� � ������.
        mov dword ptr [cpuid_string], ebx
        mov dword ptr [cpuid_string + 4], edx
        mov dword ptr [cpuid_string + 8], ecx

        ; 4. �������� ��������� ���������� � ���������� ��� ������, ��������� � �.�. ��������� ������ �������������� ���������� � ����������.
        mov eax, 1
        cpuid
        mov [cpuid_signature], eax

        ; 5. �������� ����� �������. ��������� ������ �������������� ����������� �������.
        mov [cpuid_ecx_flags], ecx
        mov [cpuid_edx_flags], edx

        end:
    }


    unsigned char first[16] = { 1, 0, 0x38, 0x7f, 0, 0x80, 1, 0 };
    unsigned char second[16] = { 0xFF, 0xFF, 0xF9, 0xE8, 1, 0, 2, 0 };
    __asm
    {
        movq MM0, qword ptr [first]
        movq MM2, qword ptr [second]
        psubsw MM0, MM2
    }

    printf("8086/88: %s\n", is_8086_88 ? "yes" : "no");
    printf("Is CPUID supported: %s\n", is_cpuid_supported ? "yes" : "no");
    printf("Maximum supported number of CPUID parameters: %d\n", cpuid_max);
    printf("CPUID string: %s\n", cpuid_string);
    printf("CPUID signature: %08X\n", cpuid_signature);
    printf("CPUID ECX flags: %08X, EDX flags: %08X\n", cpuid_ecx_flags, cpuid_edx_flags);

    printf("\n");
}


