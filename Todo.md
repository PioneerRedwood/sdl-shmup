# Feedback #1
## Premultiplied alpha blending 구현

## 픽셀로 그리기할 때, 일종의 커스텀 백버퍼에 한번만 DrawPoints 그리기로 해보기

## STL?
- 언리얼에서는 사용하지 않음
- EASTL
- 메모리 접근 방식? std::list (연결 리스트);
    - 메모리 점프하는게 일반 배열보다 느려서?
    - std::unique_ptr<TYPE> = ... std::shared_ptr<TYPE> ?
- OS; ANSI C
- OOP vs. Entity Component System (ECS)
    - 유니티에서 차용하는 시스템 Monobehavior와 다르게 해야 함
    - Bullet (pos, power, energy ...)
        - for loop 개수만큼 - 메모리 정렬?
        - CPU Cache 만큼 한번에 읽기, Hit/Miss
        - [Bullet0 ... BulletN]
        - 검증? Cache 히트는 디버깅 불가?
        - Intel Vtune Profiler
    - 루프를 빠르게 멀티스레드, 버스트 컴파일러(SIMD)

## Geometry Collision
- Sweep test (Sphere)
- Sphere, Ray and Box(AABB)
- 스위핑 테스트
- Capsule vs. Capsule

## 물리 엔진
- CCD
- 충돌과 그 뒤 계산을 빠르게 하는 것
- 마우스 픽킹

## 고정 프레임의 경우 꼼수가 있음
